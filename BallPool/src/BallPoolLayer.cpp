#include "BallPoolLayer.h"
#include "Lights/AmbientLight.h"
#include "Lights/DirectionalLight.h"
#include "Lights/SpotLight.h"
#include "Lights/ConeLight.h"

#include <Roose/Core/Application.h>
#include <Roose/Renderer/Renderer.h>
#include <Roose/Renderer/UniformBuffer.h>
#include <Roose/Events/MouseEvent.h>
#include <Roose/Input/Input.h>
#include <Roose/Scene/OrthographicCamera.h>
#include <Roose/Scene/PerspectiveCamera.h>

#include <GLFW/glfw3.h>

#include "glm/gtc/random.hpp"
#include "glm/gtx/quaternion.hpp"

static Roose::Application* App = nullptr;
static Roose::Window* Window = nullptr;
static GLFWwindow* GLFW_WindowHandle = nullptr;

struct BallPoolData
{
    struct CameraData
    {
        glm::mat4 ViewProjection;
        glm::vec4 Position;
    };

    CameraData CameraBuffer;
    Roose::Ref<Roose::UniformBuffer> CameraUniformBuffer;

    struct TransformComponent
    {
        glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
        glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
        glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;
        TransformComponent(const glm::vec3& translation)
            : Translation(translation) {}

        glm::mat4 GetTransform() const
        {
            glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

            return glm::translate(glm::mat4(1.0f), Translation)
                * rotation
                * glm::scale(glm::mat4(1.0f), Scale);
        }
    };

    Roose::Ref<Roose::Shader> BlinnPhongShader;

    // Light Sources
    AmbientLight AmbientLight;
    DirectionalLight DirectionalLight;
    SpotLight SpotLight;
    ConeLight ConeLight;

    TransformComponent CameraTransform;
    float CameraYaw = 0.0f;          // horizontal rotation in radians
    float CameraPitch = 0.0f;        // vertical rotation in radians
    float CameraOrbitRadius = 50.0f; // default fixed radius

    glm::vec2 PreviousMousePosition = { 0.0f, 0.0f };
    bool IsMouseEnabled = false;
    float TargetFOV = glm::radians(60.0f);
};

static BallPoolData s_Data;

BallPoolLayer::BallPoolLayer() : Layer("BallPoolLayer")
{
}

void BallPoolLayer::OnAttach()
{
    App = &Roose::Application::Get();
    Window = &App->GetWindow();
    GLFW_WindowHandle = Window->GetNativeWindow();

    srand(time(nullptr));

    // Hide and lock the mouse cursor
    glfwSetInputMode(GLFW_WindowHandle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // Load the billiard balls.
    // Since all billiard balls share the same mesh, we'll load one mesh and reuse it for all balls.
    // This will also reuse the same VAO, VBO, and IBO.
    // We'll load the materials separately and assign them to the corresponding ball.
    m_BilliardBalls.emplace_back("assets/models/Ball1.obj");

    float spacing = 2.05f;
    glm::vec3 rackOrigin = { 0.0f, 0.0f, 0.0f };

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

    m_BilliardBalls[0].SetPosition(ballPositions[0]);
    m_BilliardBalls[0].SetRotation({ -0.75f, 0.0f, 0.0f });
    for (uint8_t i = 1; i < 15; ++i)
    {
        m_BilliardBalls.emplace_back(m_BilliardBalls[0]); // Copy the first ball
        m_BilliardBalls[i].SetMaterial(Roose::MaterialLibrary::Load("assets/models/Ball" + std::to_string(i + 1) + ".mtl"));
        m_BilliardBalls[i].SetPosition(ballPositions[i]);
        m_BilliardBalls[i].SetRotation({ -0.75f, 0.0f, 0.0f });
    }

    m_Camera = Roose::PerspectiveCamera();
    m_Camera.SetViewportSize(App->GetWindow().GetWidth(), App->GetWindow().GetHeight());
    m_TopViewCamera = Roose::OrthographicCamera();

    s_Data.CameraUniformBuffer = Roose::UniformBuffer::Create(sizeof(BallPoolData::CameraData), 0);
    s_Data.CameraTransform.Translation = { 10.0f, 0.0f, 10.0f };

    // Setup initial light source uniforms
    s_Data.DirectionalLight.SetColor({0.94f, 0.434f, 0.79f});
    s_Data.SpotLight.SetColor({0.75f, 0.0f, 0.0f});
    s_Data.ConeLight.SetColor({0.52f, 0.19f, 0.135f});
    s_Data.BlinnPhongShader = Roose::ShaderLibrary::Get("BlinnPhong");
    s_Data.BlinnPhongShader->Bind();
    s_Data.AmbientLight.ApplyUniforms(s_Data.BlinnPhongShader, "u_AmbientLight");
    s_Data.DirectionalLight.ApplyUniforms(s_Data.BlinnPhongShader, "u_DirLight");
    s_Data.SpotLight.ApplyUniforms(s_Data.BlinnPhongShader, "u_SpotLight");
    s_Data.ConeLight.ApplyUniforms(s_Data.BlinnPhongShader, "u_ConeLight");
}

void BallPoolLayer::OnDetach()
{
}

void BallPoolLayer::OnUpdate(const Roose::Timestep deltaTime)
{
    App->GetWindow().SetTitle("Ball Pool | FPS: " + std::to_string(1.0f / deltaTime));

    s_Data.CameraBuffer.Position = glm::vec4(s_Data.CameraTransform.Translation, 0.0f);
    s_Data.CameraBuffer.ViewProjection = m_Camera.GetProjection() * glm::inverse(s_Data.CameraTransform.GetTransform());
    s_Data.CameraUniformBuffer->SetData(&s_Data.CameraBuffer, sizeof(BallPoolData::CameraData));

    // Smooth zoom
    if (abs(s_Data.TargetFOV - m_Camera.GetVerticalFOV()) > 0.01f)
    {
        float currentFOV = m_Camera.GetVerticalFOV();
        float smoothFactor = 10.0f;
        float newFOV = glm::mix(currentFOV, s_Data.TargetFOV, smoothFactor * deltaTime);
        m_Camera.SetVerticalFOV(newFOV);
    }

    s_Data.IsMouseEnabled = Roose::Input::IsKeyDown(Roose::Key::LeftAlt);
    glfwSetInputMode(GLFW_WindowHandle, GLFW_CURSOR, s_Data.IsMouseEnabled ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);

    // Update light sources
    s_Data.AmbientLight.ApplyUniforms(s_Data.BlinnPhongShader, "u_AmbientLight");
    s_Data.DirectionalLight.ApplyUniforms(s_Data.BlinnPhongShader, "u_DirLight");
    s_Data.SpotLight.ApplyUniforms(s_Data.BlinnPhongShader, "u_SpotLight");
    s_Data.ConeLight.ApplyUniforms(s_Data.BlinnPhongShader, "u_ConeLight");

    // Render
    Roose::Renderer::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
    Roose::Renderer::Clear();
    for (const auto& gameObject : m_BilliardBalls)
    {
        gameObject.Render();
    }
}

void BallPoolLayer::OnEvent(Roose::Event& e)
{
    // m_CameraController.OnEvent(e);

    Roose::EventDispatcher dispatcher(e);
    dispatcher.Dispatch<Roose::KeyDownEvent>(RS_BIND_EVENT_FN(BallPoolLayer::OnKeyDown));
    dispatcher.Dispatch<Roose::MouseMoveEvent>(RS_BIND_EVENT_FN(BallPoolLayer::OnMouseMove));
    dispatcher.Dispatch<Roose::MouseScrollEvent>(RS_BIND_EVENT_FN(BallPoolLayer::OnMouseScroll));
    dispatcher.Dispatch<Roose::WindowResizeEvent>(RS_BIND_EVENT_FN(BallPoolLayer::OnWindowResized));
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

bool BallPoolLayer::OnMouseMove(const Roose::MouseMoveEvent& e)
{
    if (s_Data.IsMouseEnabled) return false;

    glm::vec2 currentMousePosition = { e.GetX(), e.GetY() };
    glm::vec2 delta = currentMousePosition - s_Data.PreviousMousePosition;
    s_Data.PreviousMousePosition = currentMousePosition;

    float sensitivity = 0.0025f;

    // Update yaw and pitch based on mouse movement
    s_Data.CameraYaw   -= delta.x * sensitivity;
    s_Data.CameraPitch -= delta.y * sensitivity;

    // Clamp pitch to avoid flipping (e.g., straight up or down)
    s_Data.CameraPitch = glm::clamp(s_Data.CameraPitch, glm::radians(15.0f), glm::radians(60.0f));

    // Calculate new camera position using spherical coordinates
    float radius = s_Data.CameraOrbitRadius;
    float yaw = s_Data.CameraYaw;
    float pitch = s_Data.CameraPitch;

    float x = radius * cos(pitch) * cos(yaw);
    float y = radius * sin(pitch);
    float z = radius * cos(pitch) * sin(yaw);

    glm::vec3 newCameraPos = { x, y, z };
    s_Data.CameraTransform.Translation = newCameraPos;

    // Update rotation to look at the origin
    glm::mat4 viewMatrix = glm::lookAt(newCameraPos, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::quat rotationQuat = glm::quat_cast(glm::inverse(viewMatrix));
    s_Data.CameraTransform.Rotation = glm::eulerAngles(rotationQuat);

    return false;
}

bool BallPoolLayer::OnMouseScroll(const Roose::MouseScrollEvent& e)
{
    // Adjust the target field of view (FOV) for zooming
    float zoomSpeed = glm::radians(2.5f);
    s_Data.TargetFOV -= e.GetYOffset() * zoomSpeed;

    // Clamp the target FOV to prevent extreme zoom levels
    s_Data.TargetFOV = glm::clamp(s_Data.TargetFOV, glm::radians(15.0f), glm::radians(90.0f));

    return false;
}

bool BallPoolLayer::OnWindowResized(const Roose::WindowResizeEvent& e)
{
    m_Camera.SetViewportSize(e.GetWidth(), e.GetHeight());
    return false;
}
