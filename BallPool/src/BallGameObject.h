#pragma once

#include <Roose/Core/Base.h>
#include <Roose/RenderableObject.h>

#include "RenderableGameObject.h"

class BallGameObject : public RenderableGameObject
{
public:
    BallGameObject(const std::string& objModelFilePath);

    void Update(Roose::Timestep deltaTime) override;
    void FixedUpdate(Roose::Timestep fixedDeltaTime) override;

    void ApplyForce(const glm::vec3& force) { RigidBody.ApplyForce(force); }
    [[nodiscard]] float GetRadius() const { return m_Radius; }

    RigidBodyComponent RigidBody;
private:
    float m_Radius = 1.0f; // (Approximation) This is used as the radius of the sphere collider
};
