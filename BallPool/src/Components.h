#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

struct TransformComponent
{
    glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
    glm::quat Rotation = { 1.0f, 0.0f, 0.0f, 0.0f };
    glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

    TransformComponent() = default;
    TransformComponent(const TransformComponent&) = default;
    TransformComponent(const glm::vec3& translation)
        : Translation(translation) {}

    [[nodiscard]] glm::mat4 GetTransform() const
    {
        return glm::translate(glm::mat4(1.0f), Translation)
            * glm::toMat4(Rotation)
            * glm::scale(glm::mat4(1.0f), Scale);
    }

    [[nodiscard]] TransformComponent Combine(const TransformComponent& parent) const
    {
        const glm::mat4 localTransformMatrix  = GetTransform();
        const glm::mat4 parentTransformMatrix = parent.GetTransform();
        const glm::mat4 combined = parentTransformMatrix * localTransformMatrix;

        TransformComponent result;

        // Decompose combined matrix back to components
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(combined, result.Scale, result.Rotation, result.Translation, skew, perspective);

        // Make sure quaternion is normalized
        result.Rotation = glm::normalize(result.Rotation);

        return result;
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
