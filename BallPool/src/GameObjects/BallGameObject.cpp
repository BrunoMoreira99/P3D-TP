#include "BallGameObject.h"

#include <string>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

static constexpr glm::quat DefaultBallRotation = { 1.0f, -0.40f, 0.0f, 0.0f };

static constexpr float BallSpacing = 2.05f;
static constexpr glm::vec3 RackOrigin = { 0.0f, 0.0f, -20.0f };
static constexpr glm::vec3 InitialBallPositions[16] = {
    RackOrigin + glm::vec3( 0.0f,           0.0f, 32.5f),           // Cue ball
    RackOrigin + glm::vec3( 0.0f,           0.0f, 2.0f * BallSpacing),  // 1
    RackOrigin + glm::vec3(-0.5 * BallSpacing,  0.0f, BallSpacing),         // 2
    RackOrigin + glm::vec3( 0.5 * BallSpacing,  0.0f, BallSpacing),         // 3
    RackOrigin + glm::vec3(-BallSpacing,        0.0f, 0.0f),            // 4
    RackOrigin + glm::vec3( BallSpacing,        0.0f, 0.0f),            // 5
    RackOrigin + glm::vec3(-1.5f * BallSpacing, 0.0f, -BallSpacing),        // 6
    RackOrigin + glm::vec3(-0.5f * BallSpacing, 0.0f, -BallSpacing),        // 7
    RackOrigin + glm::vec3( 0.0f,           0.0f, 0.0f),            // 8
    RackOrigin + glm::vec3( 0.5f * BallSpacing, 0.0f, -BallSpacing),        // 9
    RackOrigin + glm::vec3( 1.5f * BallSpacing, 0.0f, -BallSpacing),        // 10
    RackOrigin + glm::vec3(-2.0f * BallSpacing, 0.0f, -2.0f * BallSpacing), // 11
    RackOrigin + glm::vec3(-BallSpacing,        0.0f, -2.0f * BallSpacing), // 12
    RackOrigin + glm::vec3( 0.0f,           0.0f, -2.0f * BallSpacing), // 13
    RackOrigin + glm::vec3( BallSpacing,        0.0f, -2.0f * BallSpacing), // 14
    RackOrigin + glm::vec3( 2.0f * BallSpacing, 0.0f, -2.0f * BallSpacing)  // 15
};

BallGameObject::BallGameObject(const uint8_t number, const std::string& objModelFilePath)
    : RenderableGameObject(objModelFilePath), m_BallNumber(number)
{
    Transform.Translation = InitialBallPositions[m_BallNumber];
    Transform.Rotation = DefaultBallRotation;
    RigidBody.Mass = 0.165f; // Approximate mass of a billiard ball
    RigidBody.Restitution = 0.95f;
}

void BallGameObject::SetBallNumber(const uint8_t number)
{
    m_BallNumber = number;
    Transform.Translation = InitialBallPositions[m_BallNumber];
    const std::string stringifiedNumber = std::to_string(m_BallNumber);
    auto material = Roose::MaterialLibrary::Get("poolball" + stringifiedNumber + "_material");
    if (!material) material = Roose::MaterialLibrary::Load("assets/models/Ball" + stringifiedNumber + ".mtl");
    SetMaterial(material);
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

void BallGameObject::Reset()
{
    Transform.Translation = InitialBallPositions[m_BallNumber];
    Transform.Rotation = DefaultBallRotation;
    RigidBody.Velocity = glm::vec3(0.0f);
}
