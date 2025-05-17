#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

struct TransformComponent
{
    glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
    glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
    glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

    TransformComponent() = default;
    TransformComponent(const TransformComponent&) = default;
    TransformComponent(const glm::vec3& translation)
        : Translation(translation) {}

    [[nodiscard]] glm::mat4 GetTransform() const
    {
        return glm::translate(glm::mat4(1.0f), Translation)
            * glm::toMat4(glm::quat(Rotation))
            * glm::scale(glm::mat4(1.0f), Scale);
    }
};

struct RigidBodyComponent
{
    glm::vec3 Velocity = { 0.0f, 0.0f, 0.0f };
    glm::vec3 Acceleration = { 0.0f, 0.0f, 0.0f };
    float Mass = 1.0f;
    float Restitution = 0.9f; // Coefficient of restitution (bounciness)

    void ApplyForce(const glm::vec3& force) { Acceleration += force / Mass; }
};
