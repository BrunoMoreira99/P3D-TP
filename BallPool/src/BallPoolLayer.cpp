#include "BallPoolLayer.h"
#include "Lighting/AmbientLight.h"
#include "Lighting/DirectionalLight.h"
#include "Lighting/SpotLight.h"
#include "Lighting/ConeLight.h"
#include "BallGameObject.h"

#include <Roose/Core/Application.h>
#include <Roose/Renderer/Renderer.h>
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

    CameraData CameraBuffer;
    Roose::Ref<Roose::UniformBuffer> CameraUniformBuffer;

    CameraData TopViewCameraBuffer;
    Roose::Ref<Roose::UniformBuffer> TopViewCameraUniformBuffer;
    Roose::OrthographicCamera TopViewCamera;

    Roose::Ref<Roose::Shader> BlinnPhongShader;

    // Light Sources
    AmbientLight AmbientLight;
    DirectionalLight DirectionalLight;
    SpotLight SpotLight;
    ConeLight ConeLight;

    std::vector<BallGameObject> BilliardBalls;

    float fpsTimer = 0.0f;
};

static BallPoolData s_Data;

BallPoolLayer::BallPoolLayer() : Layer("BallPoolLayer") {}

void BallPoolLayer::OnAttach()
{
    App = &Roose::Application::Get();
    App->GetWindow().SetCursorMode(Roose::WindowCursorMode::Disabled);

    // Seed the random number generator
    srand(time(nullptr));

    // Load shaders
    s_Data.BlinnPhongShader = Roose::ShaderLibrary::Load("assets/shaders/BlinnPhong.glsl");
    s_Data.UnlitShader = Roose::ShaderLibrary::Load("assets/shaders/Unlit.glsl");
    s_Data.FullscreenUnlit2DShader = Roose::ShaderLibrary::Load("assets/shaders/FullscreenUnlit2D.glsl");

    // Load the billiard balls.
    // Since all billiard balls share the same mesh, we'll load one mesh and reuse it for all balls.
    // This will also reuse the same VAO, VBO, and IBO.
    // We'll load the materials separately and assign them to the corresponding ball.
    s_Data.BilliardBalls.emplace_back("assets/models/Ball1.obj");

    constexpr float spacing = 2.05f;
    constexpr glm::vec3 rackOrigin = { 0.0f, 0.0f, 0.0f };
    constexpr glm::vec3 ballRotation = { -0.75f, 0.0f, 0.0f };

    std::unordered_map<uint8_t, glm::vec3> ballPositions = {
        { 0, rackOrigin + glm::vec3(0.0f, 0.0f, 2.0f * spacing) },
        { 1, rackOrigin + glm::vec3(-spacing / 2.0f, 0.0f, spacing * 1.0f) },
        { 2, rackOrigin + glm::vec3( spacing / 2.0f, 0.0f, spacing * 1.0f) },
        { 3, rackOrigin + glm::vec3(-spacing,        0.0f, 0.0f) },
        { 7, rackOrigin + glm::vec3(0.0f,            0.0f, 0.0f) },
        { 4, rackOrigin + glm::vec3( spacing,        0.0f, 0.0f) },
        { 5, rackOrigin + glm::vec3(-1.5f * spacing, 0.0f, -spacing) },
        { 6, rackOrigin + glm::vec3(-0.5f * spacing, 0.0f, -spacing) },
        { 8, rackOrigin + glm::vec3( 0.5f * spacing, 0.0f, -spacing) },
        { 9, rackOrigin + glm::vec3( 1.5f * spacing, 0.0f, -spacing) },
        {10, rackOrigin + glm::vec3(-2.0f * spacing, 0.0f, -2.0f * spacing) },
        {11, rackOrigin + glm::vec3(-spacing,        0.0f, -2.0f * spacing) },
        {12, rackOrigin + glm::vec3(0.0f,            0.0f, -2.0f * spacing) },
        {13, rackOrigin + glm::vec3( spacing,        0.0f, -2.0f * spacing) },
        {14, rackOrigin + glm::vec3( 2.0f * spacing, 0.0f, -2.0f * spacing) }
    };

    s_Data.BilliardBalls[0].Transform.Translation = ballPositions[0];
    s_Data.BilliardBalls[0].Transform.Rotation = ballRotation;
    for (uint8_t i = 1; i < 15; ++i)
    {
        s_Data.BilliardBalls.emplace_back(s_Data.BilliardBalls[0]); // Copy the first ball
        s_Data.BilliardBalls[i].SetMaterial(Roose::MaterialLibrary::Load("assets/models/Ball" + std::to_string(i + 1) + ".mtl"));
        s_Data.BilliardBalls[i].Transform.Translation = ballPositions[i];
        s_Data.BilliardBalls[i].Transform.Rotation = ballRotation;
    }

    // Set up the top view camera
    s_Data.TopViewCamera.SetViewportSize(App->GetWindow().GetWidth(), App->GetWindow().GetHeight());
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
    s_Data.TopViewCameraUniformBuffer = Roose::UniformBuffer::Create(sizeof(BallPoolData::CameraData), 1);
    s_Data.TopViewCameraUniformBuffer->SetData(&s_Data.TopViewCameraBuffer, sizeof(BallPoolData::CameraData));

    // Create a uniform buffer for the main camera
    s_Data.CameraUniformBuffer = Roose::UniformBuffer::Create(sizeof(BallPoolData::CameraData), 0);

    // Set up initial light source uniforms
    s_Data.DirectionalLight.SetColor({0.94f, 0.434f, 0.79f});
    s_Data.SpotLight.SetColor({0.75f, 0.0f, 0.0f});
    s_Data.ConeLight.SetColor({0.52f, 0.19f, 0.135f});
    s_Data.BlinnPhongShader->Bind();
    s_Data.AmbientLight.ApplyUniforms(s_Data.BlinnPhongShader, "u_AmbientLight");
    s_Data.DirectionalLight.ApplyUniforms(s_Data.BlinnPhongShader, "u_DirLight");
    s_Data.SpotLight.ApplyUniforms(s_Data.BlinnPhongShader, "u_SpotLight");
    s_Data.ConeLight.ApplyUniforms(s_Data.BlinnPhongShader, "u_ConeLight");
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

    // Update camera
    m_CameraController.OnUpdate(deltaTime);
    s_Data.CameraBuffer.Position = glm::vec4(m_CameraController.GetPosition(), 0.0f);
    s_Data.CameraBuffer.ViewProjection = m_CameraController.GetViewProjection();
    s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(BallPoolData::CameraData));

    // Update light sources
    s_Data.AmbientLight.ApplyUniforms(s_Data.BlinnPhongShader, "u_AmbientLight");
    s_Data.DirectionalLight.ApplyUniforms(s_Data.BlinnPhongShader, "u_DirLight");
    s_Data.SpotLight.ApplyUniforms(s_Data.BlinnPhongShader, "u_SpotLight");
    s_Data.ConeLight.ApplyUniforms(s_Data.BlinnPhongShader, "u_ConeLight");

    // Render
    Roose::Renderer::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
    Roose::Renderer::Clear();
    for (const auto& gameObject : s_Data.BilliardBalls)
    {
        gameObject.Render();
    }

    // TODO: Draw the table and minimap
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
        case Roose::Key::D1: case Roose::Key::KP1:
            handleLight(s_Data.AmbientLight);
            break;
        case Roose::Key::D2: case Roose::Key::KP2:
            handleLight(s_Data.DirectionalLight);
            break;
        case Roose::Key::D3: case Roose::Key::KP3:
            handleLight(s_Data.SpotLight);
            break;
        case Roose::Key::D4: case Roose::Key::KP4:
            handleLight(s_Data.ConeLight);
            break;
    }

    return false;
}
