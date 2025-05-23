#pragma once

#include <Roose/Core/Base.h>
#include <Roose/RenderableObject.h>

#include "RenderableGameObject.h"

class BallGameObject : public RenderableGameObject
{
public:
    BallGameObject(uint8_t number, const std::string& objModelFilePath);

    void FixedUpdate(Roose::Timestep fixedDeltaTime) override;
    void Reset() override;

    void SetBallNumber(uint8_t number);
    void ApplyForce(const glm::vec3& force) { RigidBody.ApplyForce(force); }
    [[nodiscard]] float GetRadius() const { return m_Radius; }

    RigidBodyComponent RigidBody;
private:
    uint8_t m_BallNumber;
    float m_Radius = 1.0f; // (Approximation) This is used as the radius of the sphere collider
};
