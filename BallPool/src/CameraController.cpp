#include "CameraController.h"
#include <Roose/Core/Application.h>
#include <Roose/Input/Input.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

CameraController::CameraController() : m_Window(Roose::Application::Get().GetWindow())
{
    m_Camera.SetViewportSize(m_Window.GetWidth(), m_Window.GetHeight());
    m_PrevMousePos = Roose::Input::GetMousePosition();
    UpdateCameraPosition(m_PrevMousePos);
}

void CameraController::OnUpdate(const float deltaTime)
{
    // Smooth zoom
    const float currentFOV = m_Camera.GetVerticalFOV();
    if (abs(m_TargetFOV - currentFOV) > 0.01f)
    {
        const float newFOV = glm::mix(currentFOV, m_TargetFOV, m_ZoomSmoothFactor * deltaTime);
        m_Camera.SetVerticalFOV(newFOV);
    }

    m_CameraControlEnabled = !Roose::Input::IsKeyDown(Roose::Key::LeftAlt);
    m_Window.SetCursorMode(m_CameraControlEnabled ? Roose::WindowCursorMode::Disabled : Roose::WindowCursorMode::Normal);
}

void CameraController::OnEvent(Roose::Event& e)
{
    Roose::EventDispatcher dispatcher(e);
    dispatcher.Dispatch<Roose::MouseMoveEvent>(RS_BIND_EVENT_FN(CameraController::OnMouseMove));
    dispatcher.Dispatch<Roose::MouseScrollEvent>(RS_BIND_EVENT_FN(CameraController::OnMouseScroll));
    dispatcher.Dispatch<Roose::WindowResizeEvent>(RS_BIND_EVENT_FN(CameraController::OnWindowResized));
}

void CameraController::UpdateCameraPosition(const glm::vec2& mousePosition)
{
    // Calculate the mouse delta
    const glm::vec2 delta = mousePosition - m_PrevMousePos;
    m_PrevMousePos = mousePosition;

    m_Yaw   -= delta.x * m_LookSensitivity;
    m_Pitch -= delta.y * m_LookSensitivity;
    m_Pitch = glm::clamp(m_Pitch, glm::radians(15.0f), glm::radians(60.0f));

    // Calculate the new camera position
    const float x = m_OrbitRadius * cos(m_Pitch) * cos(m_Yaw);
    const float y = m_OrbitRadius * sin(m_Pitch);
    const float z = m_OrbitRadius * cos(m_Pitch) * sin(m_Yaw);
    m_Translation = { x, y, z };
}

bool CameraController::OnMouseMove(const Roose::MouseMoveEvent& e)
{
    if (m_CameraControlEnabled)
        UpdateCameraPosition({ e.GetX(), e.GetY() });

    return false;
}

bool CameraController::OnMouseScroll(const Roose::MouseScrollEvent& e)
{
    m_TargetFOV -= e.GetYOffset() * m_ZoomSpeed;
    m_TargetFOV = glm::clamp(m_TargetFOV, 0.0f, glm::radians(90.0f));
    return false;
}

bool CameraController::OnWindowResized(const Roose::WindowResizeEvent& e)
{
    m_Camera.SetViewportSize(e.GetWidth(), e.GetHeight());
    return false;
}

glm::mat4 CameraController::GetViewProjection() const
{
    const glm::mat4 view = glm::lookAt(m_Translation, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    return m_Camera.GetProjection() * view;
}
