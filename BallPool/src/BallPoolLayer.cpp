#include "BallPoolLayer.h"
#include "Lighting/AmbientLight.h"
#include "Lighting/DirectionalLight.h"
#include "Lighting/PointLight.h"
#include "Lighting/SpotLight.h"
#include "BallGameObject.h"

#include <Roose/Core/Application.h>
#include <Roose/Renderer/Renderer.h>
#include "Roose/Renderer/Framebuffer.h"
#include <Roose/Renderer/UniformBuffer.h>
#include <Roose/Input/Input.h>
#include <Roose/Scene/OrthographicCamera.h>

#include <glm/gtc/random.hpp>
#include <glm/gtx/quaternion.hpp>

static Roose::Application* App = nullptr;

struct BallPoolData
{
    struct CameraData
    {
        glm::mat4 ViewProjection;
        glm::vec4 Position; // Must be vec4 to prevent memory alignment issues
    };

    CameraData MainCameraBuffer;
    CameraData TopViewCameraBuffer;
    Roose::OrthographicCamera TopViewCamera;
    Roose::Ref<Roose::Framebuffer> TopViewFramebuffer;

    Roose::Ref<Roose::Shader> BlinnPhongShader;
    Roose::Ref<Roose::Shader> UnlitShader;

    // Light Sources
    AmbientLight AmbientLight;
    DirectionalLight DirectionalLight;
    PointLight PointLight;
    SpotLight SpotLight;

    Roose::Scope<RenderableGameObject> TableGameObject;
    std::vector<BallGameObject> BilliardBalls;

    float fpsTimer = 0.0f;
};

static BallPoolData s_Data;

constexpr float spacing = 2.05f;
constexpr glm::vec3 rackOrigin = { 0.0f, 0.0f, -20.0f };
constexpr glm::vec3 InitialBallPositions[16] = {
    rackOrigin + glm::vec3( 0.0f,           0.0f, 32.5f),           // Cue ball
    rackOrigin + glm::vec3( 0.0f,           0.0f, 2.0f * spacing),  // 1
    rackOrigin + glm::vec3(-0.5 * spacing,  0.0f, spacing),         // 2
    rackOrigin + glm::vec3( 0.5 * spacing,  0.0f, spacing),         // 3
    rackOrigin + glm::vec3(-spacing,        0.0f, 0.0f),            // 4
    rackOrigin + glm::vec3( spacing,        0.0f, 0.0f),            // 5
    rackOrigin + glm::vec3(-1.5f * spacing, 0.0f, -spacing),        // 6
    rackOrigin + glm::vec3(-0.5f * spacing, 0.0f, -spacing),        // 7
    rackOrigin + glm::vec3( 0.0f,           0.0f, 0.0f),            // 8
    rackOrigin + glm::vec3( 0.5f * spacing, 0.0f, -spacing),        // 9
    rackOrigin + glm::vec3( 1.5f * spacing, 0.0f, -spacing),        // 10
    rackOrigin + glm::vec3(-2.0f * spacing, 0.0f, -2.0f * spacing), // 11
    rackOrigin + glm::vec3(-spacing,        0.0f, -2.0f * spacing), // 12
    rackOrigin + glm::vec3( 0.0f,           0.0f, -2.0f * spacing), // 13
    rackOrigin + glm::vec3( spacing,        0.0f, -2.0f * spacing), // 14
    rackOrigin + glm::vec3( 2.0f * spacing, 0.0f, -2.0f * spacing)  // 15
};
constexpr glm::vec3 InitialBallRotation = { -0.75f, 0.0f, 0.0f };

constexpr uint32_t TopViewFramebufferWidth = 540;
constexpr uint32_t TopViewFramebufferHeight = 360;
constexpr float TopViewFramebufferAspectRatio = 540.0f / 360.0f;

static void ResetBilliardBalls()
{
    for (size_t i = 0; i < s_Data.BilliardBalls.size(); ++i)
    {
        s_Data.BilliardBalls[i].Transform.Translation = InitialBallPositions[i];
        s_Data.BilliardBalls[i].Transform.Rotation = InitialBallRotation;
        s_Data.BilliardBalls[i].RigidBody.Velocity = glm::vec3(0.0f);
    }
}

BallPoolLayer::BallPoolLayer() : Layer("BallPoolLayer") {}

void BallPoolLayer::OnAttach()
{
    App = &Roose::Application::Get();
    App->GetWindow().SetCursorMode(Roose::WindowCursorMode::Disabled);

    // Seed the random number generator
    srand(static_cast<uint32_t>(time(nullptr)));

    // Load shaders
    s_Data.BlinnPhongShader = Roose::ShaderLibrary::Load("assets/shaders/BlinnPhong.glsl");
    s_Data.UnlitShader = Roose::ShaderLibrary::Load("assets/shaders/Unlit.glsl");

    // Load table
    s_Data.TableGameObject = Roose::CreateScope<RenderableGameObject>("assets/models/Table.obj");

    // Load the billiard balls.
    // Since all billiard balls share the same mesh, we'll load one mesh and reuse it for all balls.
    // This will also reuse the same VAO, VBO, and IBO.
    // We'll load the materials separately and assign them to the corresponding ball.
    s_Data.BilliardBalls.emplace_back("assets/models/Ball0.obj");
    s_Data.BilliardBalls[0].Transform.Translation = InitialBallPositions[0];
    s_Data.BilliardBalls[0].Transform.Rotation = InitialBallRotation;
    for (uint8_t i = 1; i < 16; ++i)
    {
        s_Data.BilliardBalls.emplace_back(s_Data.BilliardBalls[0]); // Copy the first ball
        s_Data.BilliardBalls[i].SetMaterial(Roose::MaterialLibrary::Load("assets/models/Ball" + std::to_string(i) + ".mtl"));
        s_Data.BilliardBalls[i].Transform.Translation = InitialBallPositions[i];
        s_Data.BilliardBalls[i].Transform.Rotation = InitialBallRotation;
    }

    // Set up the top view camera
    s_Data.TopViewCamera.SetViewportSize(TopViewFramebufferWidth, TopViewFramebufferHeight);
    s_Data.TopViewCamera.SetSize(50.0f);
    glm::mat4 topViewCameraView = glm::lookAt(
        glm::vec3(0.0f, 10.0f, 0.0f), // Camera position
        glm::vec3(0.0f, 0.0f, 0.0f),  // Target
        glm::vec3(0.0f, 0.0f, 1.0f)   // Up vector
    );
    // Apply a rotation around the Z-axis
    constexpr float topViewCameraRotation = glm::radians(-90.0f);
    const glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0f), topViewCameraRotation, glm::vec3(0.0f, 0.0f, 1.0f));
    // Combine the rotation with the view matrix
    topViewCameraView = rotationMatrix * topViewCameraView;
    s_Data.TopViewCameraBuffer.ViewProjection = s_Data.TopViewCamera.GetProjection() * topViewCameraView;
    s_Data.TopViewFramebuffer = Roose::Framebuffer::Create({
        TopViewFramebufferWidth, TopViewFramebufferHeight,
        { Roose::FramebufferTextureFormat::RGBA8, Roose::FramebufferTextureFormat::Depth }
    });

    // Set up initial light source uniforms
    s_Data.DirectionalLight.SetColor({0.94f, 0.434f, 0.79f});
    s_Data.PointLight.SetPosition({10.0f, 5.0f, -7.5f});
    s_Data.PointLight.SetColor({0.52f, 0.19f, 0.135f});
    s_Data.SpotLight.SetPosition({0.0f, 5.0f, -20.0f});
    s_Data.SpotLight.SetColor({0.75f, 0.0f, 0.0f});
    s_Data.AmbientLight.ApplyUniforms(s_Data.BlinnPhongShader, "u_AmbientLight");
    s_Data.DirectionalLight.ApplyUniforms(s_Data.BlinnPhongShader, "u_DirLight");
    s_Data.PointLight.ApplyUniforms(s_Data.BlinnPhongShader, "u_PointLight");
    s_Data.SpotLight.ApplyUniforms(s_Data.BlinnPhongShader, "u_SpotLight");
}

void BallPoolLayer::OnDetach() {}

void BallPoolLayer::OnUpdate(const Roose::Timestep deltaTime)
{
    s_Data.fpsTimer += deltaTime;
    if (s_Data.fpsTimer > 0.25f)
    {
        App->GetWindow().SetTitle("Ball Pool | FPS: " + std::to_string(static_cast<int32_t>(1.0f / deltaTime)));
        s_Data.fpsTimer = 0.0f;
    }

    for (auto& gameObject : s_Data.BilliardBalls)
    {
        gameObject.Update(deltaTime);
    }

    // Update camera
    m_CameraController.OnUpdate(deltaTime);
    s_Data.MainCameraBuffer.Position = glm::vec4(m_CameraController.GetPosition(), 0.0f);
    s_Data.MainCameraBuffer.ViewProjection = m_CameraController.GetViewProjection();
    Roose::Renderer::GetCameraUniformBuffer()->SetData(&s_Data.MainCameraBuffer, sizeof(BallPoolData::CameraData));

    // Update light sources
    s_Data.AmbientLight.ApplyUniforms(s_Data.BlinnPhongShader, "u_AmbientLight");
    s_Data.DirectionalLight.ApplyUniforms(s_Data.BlinnPhongShader, "u_DirLight");
    s_Data.PointLight.ApplyUniforms(s_Data.BlinnPhongShader, "u_PointLight");
    s_Data.SpotLight.ApplyUniforms(s_Data.BlinnPhongShader, "u_SpotLight");

    // Render
    Roose::Renderer::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
    Roose::Renderer::Clear();
    s_Data.BlinnPhongShader->Bind();
    s_Data.TableGameObject->Render();
    for (const auto& gameObject : s_Data.BilliardBalls)
    {
        gameObject.Render();
    }

    // Draw minimap
    // Note: We could technically use a framebuffer with two color attachments and a shader that outputs to both (MRT)
    // to render the scene and the minimap in one pass, but for simplicity we'll just render the scene again.
    Roose::Renderer::GetCameraUniformBuffer()->SetData(&s_Data.TopViewCameraBuffer, sizeof(BallPoolData::CameraData));
    s_Data.TopViewFramebuffer->Bind();
    Roose::Renderer::SetClearColor({ 0.0f, 0.0f, 0.0f, 0.0f });
    Roose::Renderer::Clear();
    s_Data.UnlitShader->Bind();
    s_Data.TableGameObject->Render();
    for (const auto& gameObject : s_Data.BilliardBalls)
    {
        gameObject.Render();
    }
    s_Data.TopViewFramebuffer->Unbind();
    Roose::Renderer::SetViewport(0, 0, App->GetWindow().GetWidth(), App->GetWindow().GetHeight());
    const uint32_t textureID = s_Data.TopViewFramebuffer->GetColorAttachmentRendererID();
    glBindTextureUnit(0, textureID);
    const float screenWidth = static_cast<float>(App->GetWindow().GetWidth());
    const float minimapScale = screenWidth / 1600.0f;
    const float minimapWidth  = 320.0f * minimapScale;
    const float minimapHeight = minimapWidth / TopViewFramebufferAspectRatio;
    const float padding = 25.0f * minimapScale;
    const float minimapX = minimapWidth / 2.0f + padding;
    const float minimapY = minimapHeight / 2.0f + padding;
    Roose::Renderer::DrawQuad(minimapX, minimapY, minimapWidth, minimapHeight);
}

void BallPoolLayer::OnFixedUpdate(const Roose::Timestep fixedDeltaTime)
{
    // Collision resolution between balls
    const float radius = s_Data.BilliardBalls[0].GetRadius();
    const float minDist = 2.0f * radius;

    // Resolve collisions for each unique pair
    for (size_t i = 0; i < s_Data.BilliardBalls.size(); ++i)
    {
        for (size_t j = i + 1; j < s_Data.BilliardBalls.size(); ++j)
        {
            auto& ballA = s_Data.BilliardBalls[i];
            auto& ballB = s_Data.BilliardBalls[j];

            const glm::vec3 delta = ballB.Transform.Translation - ballA.Transform.Translation;
            float dist2 = glm::dot(delta, delta);

            if (dist2 < minDist * minDist)
            {
                float dist = glm::sqrt(dist2);
                if (dist < 1e-6f) dist = minDist; // Prevent division by zero

                // Position correction (minimum translation distance)
                const float penetration = minDist - dist;
                const glm::vec3 correctionDir = delta / dist;
                const glm::vec3 correction = correctionDir * (penetration * 0.5f);

                ballA.Transform.Translation -= correction;
                ballB.Transform.Translation += correction;

                // Velocity response (elastic collision)
                const glm::vec3 vA = ballA.RigidBody.Velocity;
                const glm::vec3 vB = ballB.RigidBody.Velocity;
                const float mA = ballA.RigidBody.Mass;
                const float mB = ballB.RigidBody.Mass;
                const float restitution = glm::min(ballA.RigidBody.Restitution, ballB.RigidBody.Restitution);

                const glm::vec3 normal = correctionDir;
                const float vRel = glm::dot(vB - vA, normal);

                if (vRel < 0.0f) // Only resolve if balls are moving towards each other
                {
                    const float impulseMag = -(1.0f + restitution) * vRel / (1.0f / mA + 1.0f / mB);
                    glm::vec3 impulse = impulseMag * normal;

                    ballA.RigidBody.Velocity -= impulse / mA;
                    ballB.RigidBody.Velocity += impulse / mB;
                }
            }
        }
    }

    // Table boundaries
    constexpr float tableMinX = -19.6181f;
    constexpr float tableMaxX =  19.6181f;
    constexpr float tableMinZ = -29.4118f;
    constexpr float tableMaxZ =  29.4118f;

    // Ball-table boundary collision
    for (auto& gameObject : s_Data.BilliardBalls)
    {
        glm::vec3& pos = gameObject.Transform.Translation;
        glm::vec3& vel = gameObject.RigidBody.Velocity;
        float restitution = gameObject.RigidBody.Restitution;

        // X boundaries
        if (pos.x - radius < tableMinX)
        {
            pos.x = tableMinX + radius;
            if (vel.x < 0.0f)
                vel.x = -vel.x * restitution;
        }
        else if (pos.x + radius > tableMaxX)
        {
            pos.x = tableMaxX - radius;
            if (vel.x > 0.0f)
                vel.x = -vel.x * restitution;
        }

        // Z boundaries
        if (pos.z - radius < tableMinZ)
        {
            pos.z = tableMinZ + radius;
            if (vel.z < 0.0f)
                vel.z = -vel.z * restitution;
        }
        else if (pos.z + radius > tableMaxZ)
        {
            pos.z = tableMaxZ - radius;
            if (vel.z > 0.0f)
                vel.z = -vel.z * restitution;
        }
    }

    // Update physics for each ball
    for (auto& gameObject : s_Data.BilliardBalls)
    {
        gameObject.FixedUpdate(fixedDeltaTime);
    }
}

void BallPoolLayer::OnEvent(Roose::Event& e)
{
    m_CameraController.OnEvent(e);

    Roose::EventDispatcher dispatcher(e);
    dispatcher.Dispatch<Roose::KeyDownEvent>(RS_BIND_EVENT_FN(BallPoolLayer::OnKeyDown));
}

bool BallPoolLayer::OnKeyDown(const Roose::KeyDownEvent& e)
{
    if (e.IsRepeat()) return false;

    const bool ctrl = Roose::Input::IsKeyDown(Roose::Key::LeftControl) || Roose::Input::IsKeyDown(Roose::Key::RightControl);
    const bool shift = Roose::Input::IsKeyDown(Roose::Key::LeftShift) || Roose::Input::IsKeyDown(Roose::Key::RightShift);

    const auto handleLight = [&](auto& light) {
        if (shift)
            light.SetColor({ 1.0f, 1.0f, 1.0f });
        else if (ctrl)
            light.SetColor({ glm::linearRand(0.0f, 1.0f), glm::linearRand(0.0f, 1.0f), glm::linearRand(0.0f, 1.0f) });
        else
            light.Toggle();
    };

    switch (e.GetKeyCode())
    {
        case Roose::Key::Escape:
            App->Close();
            break;
        case Roose::Key::Enter:
            if (ctrl) App->GetWindow().ToggleFullscreen();
            break;
        case Roose::Key::Space:
            // Apply force to the cue ball
            s_Data.BilliardBalls[0].ApplyForce({ 0.0f, 0.0f, -750.0f });
            break;
        case Roose::Key::R:
            ResetBilliardBalls();
            break;
        case Roose::Key::D1: case Roose::Key::KP1:
            handleLight(s_Data.AmbientLight);
            break;
        case Roose::Key::D2: case Roose::Key::KP2:
            handleLight(s_Data.DirectionalLight);
            break;
        case Roose::Key::D3: case Roose::Key::KP3:
            handleLight(s_Data.PointLight);
            break;
        case Roose::Key::D4: case Roose::Key::KP4:
            handleLight(s_Data.SpotLight);
            break;
    }

    return false;
}
