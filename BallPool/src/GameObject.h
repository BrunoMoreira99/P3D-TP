#pragma once

#include <Roose/Core/Time.h>

#include "Components.h"

class GameObject
{
public:
    GameObject() = default;
    GameObject(const GameObject&) = default;
    virtual ~GameObject() = default;

    virtual void Update(Roose::Timestep deltaTime) {}
    virtual void FixedUpdate(Roose::Timestep fixedDeltaTime) {}

    TransformComponent Transform;
};
