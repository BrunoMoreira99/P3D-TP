#pragma once

#include <Roose/Core/Layer.h>
#include <Roose/Events/ApplicationEvent.h>
#include <Roose/Events/KeyEvent.h>
#include <Roose/Events/MouseEvent.h>
#include <Roose/Scene/OrthographicCamera.h>
#include <Roose/Scene/PerspectiveCamera.h>
#include "BallGameObject.h"

class BallPoolLayer : public Roose::Layer
{
public:
    BallPoolLayer();
    ~BallPoolLayer() override = default;

    void OnAttach() override;
    void OnDetach() override;

    void OnUpdate(Roose::Timestep deltaTime) override;

    void OnEvent(Roose::Event& e) override;
    bool OnKeyDown(const Roose::KeyDownEvent& e);
    bool OnMouseMove(const Roose::MouseMoveEvent& e);
    bool OnMouseScroll(const Roose::MouseScrollEvent& e);
    bool OnWindowResized(const Roose::WindowResizeEvent& e);
private:
    Roose::PerspectiveCamera m_Camera;
    Roose::OrthographicCamera m_TopViewCamera;
    std::vector<BallGameObject> m_BilliardBalls;
};
