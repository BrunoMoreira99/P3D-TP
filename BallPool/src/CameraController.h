#pragma once

#include <Roose/Scene/PerspectiveCamera.h>
#include <Roose/Events/ApplicationEvent.h>
#include <Roose/Events/Event.h>
#include <Roose/Events/MouseEvent.h>

#include <glm/glm.hpp>

namespace Roose {
    class Window; // Forward declaration
}

class CameraController
{
public:
    CameraController();

    void OnUpdate(float deltaTime);
    void OnEvent(Roose::Event& e);

    [[nodiscard]] const glm::vec3& GetPosition() const { return m_Translation; }

    [[nodiscard ]]glm::mat4 GetViewProjection() const;
private:
    bool OnMouseScroll(const Roose::MouseScrollEvent& e);
    bool OnWindowResized(const Roose::WindowResizeEvent& e);
private:
    Roose::PerspectiveCamera m_Camera;
    Roose::Window& m_Window;

    glm::vec3 m_Translation = { 0.0f, 25.0f, 70.0f };

    bool m_CameraControlEnabled = false;
    float m_TargetFOV = glm::radians(45.0f);
    float m_ZoomSmoothFactor = 10.0f;
    float m_ZoomSpeed = glm::radians(2.5f);
};
