#include "BallPoolLayer.h"
#include "Lighting/AmbientLight.h"
#include "Lighting/DirectionalLight.h"
#include "Lighting/PointLight.h"
#include "Lighting/SpotLight.h"
#include "GameObjects/GameObject.h"
#include "GameObjects/BallGameObject.h"
#include "GameObjects/LightSourceGameObject.h"

#include <Roose/Core/Application.h>
#include <Roose/Renderer/Renderer.h>
#include "Roose/Renderer/Framebuffer.h"
#include <Roose/Renderer/UniformBuffer.h>
#include <Roose/Input/Input.h>
#include <Roose/Scene/OrthographicCamera.h>

#include <glm/gtc/random.hpp>
#include <glm/gtx/quaternion.hpp>

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

    Roose::Ref<AmbientLight> AmbientLight;
    Roose::Ref<DirectionalLight> DirectionalLight;
    Roose::Ref<PointLight> PointLight;
    Roose::Ref<SpotLight> SpotLight;

    Roose::Scope<GameObject> SceneObject;
    std::vector<Roose::Ref<BallGameObject>> BilliardBalls;

    float SceneYaw = glm::radians(90.0f);
    float ScenePitch = 0.25f;
    glm::vec2 PrevMousePos = { 0.0f, 0.0f };

    float fpsTimer = 0.0f;
};

static BallPoolData s_Data;

static constexpr float LookSensitivity = 0.0025f;
static constexpr uint32_t TopViewFramebufferWidth = 540;
static constexpr uint32_t TopViewFramebufferHeight = 360;
static constexpr float TopViewFramebufferAspectRatio = 540.0f / 360.0f;

BallPoolLayer::BallPoolLayer() : Layer("BallPoolLayer") {}

void BallPoolLayer::OnAttach()
{
    Roose::Application::Get().GetWindow().SetCursorMode(Roose::WindowCursorMode::Disabled);

    // Seed the random number generator
    srand(static_cast<uint32_t>(time(nullptr)));

    // Load shaders
    s_Data.BlinnPhongShader = Roose::ShaderLibrary::Load("assets/shaders/BlinnPhong.glsl");
    s_Data.UnlitShader = Roose::ShaderLibrary::Load("assets/shaders/Unlit.glsl");

    // Create scene root game object
    s_Data.SceneObject = Roose::CreateScope<GameObject>();

    // Create lights
    s_Data.AmbientLight = Roose::CreateRef<AmbientLight>();
    s_Data.DirectionalLight = Roose::CreateRef<DirectionalLight>();
    s_Data.PointLight = Roose::CreateRef<PointLight>();
    s_Data.SpotLight = Roose::CreateRef<SpotLight>();

    const auto ambientLightGameObject = Roose::CreateRef<LightSourceGameObject>(s_Data.AmbientLight);
    const auto directionalLightGameObject = Roose::CreateRef<LightSourceGameObject>(s_Data.DirectionalLight);
    const auto pointLightGameObject = Roose::CreateRef<LightSourceGameObject>(s_Data.PointLight);
    const auto spotLightGameObject = Roose::CreateRef<LightSourceGameObject>(s_Data.SpotLight);

    s_Data.SceneObject->AddChild(ambientLightGameObject);
    s_Data.SceneObject->AddChild(directionalLightGameObject);
    s_Data.SceneObject->AddChild(pointLightGameObject);
    s_Data.SceneObject->AddChild(spotLightGameObject);

    // Set up initial light source parameters
    s_Data.DirectionalLight->SetColor({ 0.94f, 0.434f, 0.79f });
    s_Data.PointLight->SetColor({ 0.52f, 0.19f, 0.135f });
    s_Data.SpotLight->SetColor({ 0.75f, 0.0f, 0.0f });
    pointLightGameObject->Transform.Translation = { 10.0f, 5.0f, -7.5f };
    spotLightGameObject->Transform.Translation = { 0.0f, 5.0f, -20.0f };

    // Load table
    const Roose::Ref<RenderableGameObject> tableGameObject = Roose::CreateRef<RenderableGameObject>("assets/models/Table.obj");
    s_Data.SceneObject->AddChild(tableGameObject);

    // Load the billiard balls.
    // Since all billiard balls share the same mesh, we'll load one mesh and reuse it for all balls.
    // This will also reuse the same VAO, VBO, and IBO.
    // We'll load the materials separately and assign them to the corresponding ball.
    s_Data.BilliardBalls.push_back(Roose::CreateRef<BallGameObject>(0, "assets/models/Ball0.obj"));
    s_Data.SceneObject->AddChild(s_Data.BilliardBalls[0]);
    for (uint8_t i = 1; i < 16; ++i)
    {
        auto ball = Roose::CreateRef<BallGameObject>(*s_Data.BilliardBalls[0]);
        ball->SetBallNumber(i);
        s_Data.BilliardBalls.push_back(ball);
        s_Data.SceneObject->AddChild(ball);
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

    s_Data.PrevMousePos = Roose::Input::GetMousePosition();
    UpdateSceneRotation(s_Data.PrevMousePos);
}

void BallPoolLayer::OnDetach() {}

void BallPoolLayer::OnUpdate(const Roose::Timestep deltaTime)
{
    s_Data.fpsTimer += deltaTime;
    if (s_Data.fpsTimer > 0.25f)
    {
        Roose::Application::Get().GetWindow().SetTitle("Ball Pool | FPS: " + std::to_string(static_cast<int32_t>(1.0f / deltaTime)));
        s_Data.fpsTimer = 0.0f;
    }

    // Update scene
    s_Data.SceneObject->Update(deltaTime);

    // Update camera
    m_CameraController.OnUpdate(deltaTime);
    s_Data.MainCameraBuffer.Position = glm::vec4(m_CameraController.GetPosition(), 0.0f);
    s_Data.MainCameraBuffer.ViewProjection = m_CameraController.GetViewProjection();
    Roose::Renderer::GetCameraUniformBuffer()->SetData(&s_Data.MainCameraBuffer, sizeof(BallPoolData::CameraData));

    // Update light sources
    s_Data.AmbientLight->ApplyUniforms(s_Data.BlinnPhongShader, "u_AmbientLight");
    s_Data.DirectionalLight->ApplyUniforms(s_Data.BlinnPhongShader, "u_DirLight");
    s_Data.PointLight->ApplyUniforms(s_Data.BlinnPhongShader, "u_PointLight");
    s_Data.SpotLight->ApplyUniforms(s_Data.BlinnPhongShader, "u_SpotLight");

    // Render
    Roose::Renderer::SetClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
    Roose::Renderer::Clear();
    s_Data.BlinnPhongShader->Bind();
    s_Data.SceneObject->Render();

    // Draw minimap
    DrawMinimap();
}

void BallPoolLayer::DrawMinimap()
{
    // Save the current scene rotation
    const glm::quat savedSceneRotation = s_Data.SceneObject->Transform.Rotation;
    // Reset the scene rotation for the top-down minimap view
    s_Data.SceneObject->Transform.Rotation = { 1.0f, 0.0f, 0.0f, 0.0f };

    // Clear the depth buffer from the default framebuffer to avoid depth issues with the minimap
    Roose::Renderer::ClearDepth();

    // Update the top view camera uniform buffer
    Roose::Renderer::GetCameraUniformBuffer()->SetData(&s_Data.TopViewCameraBuffer, sizeof(BallPoolData::CameraData));

    // Bind the top view framebuffer and clear it
    s_Data.TopViewFramebuffer->Bind();
    Roose::Renderer::SetClearColor({ 0.0f, 0.0f, 0.0f, 0.0f });
    Roose::Renderer::Clear();

    // Render the scene from the top view using the unlit shader
    s_Data.UnlitShader->Bind();
    s_Data.SceneObject->Render();

    // Unbind the framebuffer
    s_Data.TopViewFramebuffer->Unbind();

    // Restore the original scene rotation
    s_Data.SceneObject->Transform.Rotation = savedSceneRotation;

    // Restore the viewport to the full window size
    const Roose::Window& window = Roose::Application::Get().GetWindow();
    const uint32_t windowWidth  = window.GetWidth();
    const uint32_t windowHeight = window.GetHeight();
    Roose::Renderer::SetViewport(0, 0, windowWidth, windowHeight);

    // Bind the minimap texture
    const uint32_t textureID = s_Data.TopViewFramebuffer->GetColorAttachmentRendererID();
    glBindTextureUnit(0, textureID);

    // Calculate minimap size and position
    const float screenWidth = static_cast<float>(windowWidth);
    const float minimapScale = screenWidth / 1600.0f;
    const float minimapWidth  = 320.0f * minimapScale;
    const float minimapHeight = minimapWidth / TopViewFramebufferAspectRatio;
    const float padding = 25.0f * minimapScale;
    const float minimapX = screenWidth - minimapWidth / 2.0f - padding;
    const float minimapY = minimapHeight / 2.0f + padding;

    // Draw the minimap quad
    Roose::Renderer::DrawQuad(minimapX, minimapY, minimapWidth, minimapHeight);
}

void BallPoolLayer::OnFixedUpdate(const Roose::Timestep fixedDeltaTime)
{
    // Collision resolution between balls
    const float radius  = s_Data.BilliardBalls[0]->GetRadius();
    const float minDist = 2.0f * radius;

    // Resolve collisions for each unique pair
    for (size_t i = 0; i < s_Data.BilliardBalls.size(); ++i)
    {
        for (size_t j = i + 1; j < s_Data.BilliardBalls.size(); ++j)
        {
            auto& ballA = s_Data.BilliardBalls[i];
            auto& ballB = s_Data.BilliardBalls[j];

            const glm::vec3 delta = ballB->Transform.Translation - ballA->Transform.Translation;
            const float distSquared = glm::dot(delta, delta);

            if (distSquared < minDist * minDist)
            {
                const float dist = glm::sqrt(distSquared);

                glm::vec3 correctionDir;
                if (dist < 1e-6f) // Balls are almost at the same position, we'll use a random direction to avoid division by zero
                    correctionDir = glm::normalize(glm::vec3(glm::linearRand(-1.0f, 1.0f), 0.0f, glm::linearRand(-1.0f, 1.0f)));
                else
                    correctionDir = delta / dist;

                // Position correction (minimum translation distance)
                const float penetration = minDist - dist;
                const glm::vec3 correction = correctionDir * (penetration * 0.5f);

                ballA->Transform.Translation -= correction;
                ballB->Transform.Translation += correction;

                // Velocity response (elastic collision)
                const glm::vec3 vA = ballA->RigidBody.Velocity;
                const glm::vec3 vB = ballB->RigidBody.Velocity;
                const float mA = ballA->RigidBody.Mass;
                const float mB = ballB->RigidBody.Mass;
                const float restitution = glm::min(ballA->RigidBody.Restitution, ballB->RigidBody.Restitution);

                const float vRel = glm::dot(vB - vA, correctionDir);

                if (vRel < 0.0f) // Only resolve if balls are moving towards each other
                {
                    const float impulseMag = -(1.0f + restitution) * vRel / (1.0f / mA + 1.0f / mB);
                    glm::vec3 impulse = impulseMag * correctionDir;

                    ballA->RigidBody.Velocity -= impulse / mA;
                    ballB->RigidBody.Velocity += impulse / mB;
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
        glm::vec3& pos = gameObject->Transform.Translation;
        glm::vec3& vel = gameObject->RigidBody.Velocity;
        float restitution = gameObject->RigidBody.Restitution;

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
        gameObject->FixedUpdate(fixedDeltaTime);
    }
}

void BallPoolLayer::OnEvent(Roose::Event& e)
{
    m_CameraController.OnEvent(e);

    Roose::EventDispatcher dispatcher(e);
    dispatcher.Dispatch<Roose::KeyDownEvent>(RS_BIND_EVENT_FN(BallPoolLayer::OnKeyDown));
    dispatcher.Dispatch<Roose::MouseMoveEvent>(RS_BIND_EVENT_FN(BallPoolLayer::OnMouseMove));
}

bool BallPoolLayer::OnKeyDown(const Roose::KeyDownEvent& e)
{
    if (e.IsRepeat()) return false;

    const bool ctrl = Roose::Input::IsKeyDown(Roose::Key::LeftControl) || Roose::Input::IsKeyDown(Roose::Key::RightControl);
    const bool shift = Roose::Input::IsKeyDown(Roose::Key::LeftShift) || Roose::Input::IsKeyDown(Roose::Key::RightShift);

    const auto handleLight = [&](const Roose::Ref<BaseLight>& light) {
        if (shift)
            light->SetColor({ 1.0f, 1.0f, 1.0f });
        else if (ctrl)
            light->SetColor({ glm::linearRand(0.0f, 1.0f), glm::linearRand(0.0f, 1.0f), glm::linearRand(0.0f, 1.0f) });
        else
            light->Toggle();
    };

    switch (e.GetKeyCode())
    {
        case Roose::Key::Escape:
            Roose::Application::Get().Close();
            break;
        case Roose::Key::Enter:
            if (ctrl) Roose::Application::Get().GetWindow().ToggleFullscreen();
            break;
        case Roose::Key::Space:
            // Apply force to the cue ball
            s_Data.BilliardBalls[0]->ApplyForce({ 0.0f, 0.0f, -750.0f });
            break;
        case Roose::Key::R:
            s_Data.SceneObject->Reset();
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

bool BallPoolLayer::OnMouseMove(const Roose::MouseMoveEvent& e)
{
    if (!Roose::Input::IsKeyDown(Roose::Key::LeftAlt))
        UpdateSceneRotation({ e.GetX(), e.GetY() });

    return false;
}

void BallPoolLayer::UpdateSceneRotation(const glm::vec2& mousePosition)
{
    const glm::vec2 delta = mousePosition - s_Data.PrevMousePos;
    s_Data.PrevMousePos = mousePosition;

    s_Data.SceneYaw   -= delta.x * LookSensitivity;
    s_Data.ScenePitch -= delta.y * LookSensitivity;
    s_Data.ScenePitch  = glm::clamp(s_Data.ScenePitch, glm::radians(-5.0f), glm::radians(30.0f));

    // Yaw rotation (around Y axis)
    const glm::quat yawRotation = glm::angleAxis(s_Data.SceneYaw, glm::vec3(0, 1, 0));

    // Get right axis for pitch relative to the main camera
    const glm::vec3 cameraPos = m_CameraController.GetPosition();
    const glm::vec3 objectPos = s_Data.SceneObject->Transform.Translation;
    const glm::vec3 toCamera = glm::normalize(cameraPos - objectPos);

    const glm::vec3 rightAxis = glm::normalize(glm::cross(glm::vec3(0, 1, 0), toCamera));

    // Pitch around right axis
    const glm::quat pitchRotation = glm::angleAxis(s_Data.ScenePitch, rightAxis);

    s_Data.SceneObject->Transform.Rotation = pitchRotation * yawRotation;
}
