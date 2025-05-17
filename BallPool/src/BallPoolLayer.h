#pragma once

#include <Roose/Core/Layer.h>
#include <Roose/Events/ApplicationEvent.h>
#include <Roose/Events/KeyEvent.h>

#include "CameraController.h"

class BallPoolLayer : public Roose::Layer
{
public:
    BallPoolLayer();
    ~BallPoolLayer() override = default;

    void OnAttach() override;
    void OnDetach() override;

    void OnUpdate(Roose::Timestep deltaTime) override;
    void OnFixedUpdate(Roose::Timestep fixedDeltaTime) override;

    void OnEvent(Roose::Event& e) override;
    bool OnKeyDown(const Roose::KeyDownEvent& e);
private:
    CameraController m_CameraController;
};
