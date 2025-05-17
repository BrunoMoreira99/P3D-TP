#include "BallGameObject.h"
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

BallGameObject::BallGameObject(const std::string& objModelFilePath)
{
    m_RenderableObject = Roose::CreateRef<Roose::RenderableObject>();
    m_RenderableObject->Load(objModelFilePath);
    m_RenderableObject->Install();
    m_Material = m_RenderableObject->GetMeshesEntries()[0].Material;
    RigidBody.Mass = 0.165f; // Approximate mass of a billiard ball
    RigidBody.Restitution = 0.95f;
}

void BallGameObject::Update(const Roose::Timestep deltaTime)
{
}

void BallGameObject::FixedUpdate(const Roose::Timestep fixedDeltaTime)
{
    // RigidBody physics simulation
    RigidBody.Velocity += RigidBody.Acceleration * fixedDeltaTime.GetSeconds();
    Transform.Translation += RigidBody.Velocity * fixedDeltaTime.GetSeconds();
    RigidBody.Velocity *= 0.99f; // Friction
    RigidBody.Acceleration = glm::vec3(0.0f);

    // Ball rolling
    const glm::vec3 horizontalVel = { RigidBody.Velocity.x, 0.0f, RigidBody.Velocity.z };
    const float speed = glm::length(horizontalVel);
    if (speed > 0.0001f) {
        // Axis of rotation is perpendicular to velocity in XZ plane (right-hand rule)
        const glm::vec3 axis = glm::normalize(glm::cross(glm::vec3(0, 1, 0), horizontalVel));

        const float distance = speed * fixedDeltaTime;
        const float angularDistance = distance / m_Radius;

        // Convert current Euler rotation to quaternion
        const glm::quat currentQuat = glm::quat(Transform.Rotation);
        // Create a quaternion representing the incremental rotation
        const glm::quat deltaQuat = glm::angleAxis(angularDistance, axis);
        // Apply the incremental rotation
        const glm::quat newQuat = glm::normalize(deltaQuat * currentQuat);

        // Convert back to Euler angles (radians)
        Transform.Rotation = glm::eulerAngles(newQuat);
    }
}

void BallGameObject::Render() const
{
    m_RenderableObject->SetMaterial(m_Material);
    m_RenderableObject->Render(Transform.Translation, Transform.Rotation);
}
