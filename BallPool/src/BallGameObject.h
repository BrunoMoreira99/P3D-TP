#pragma once

#include <Roose/Core/Base.h>
#include <Roose/RenderableObject.h>

#include "GameObject.h"

class BallGameObject : public GameObject
{
public:
    BallGameObject(const std::string& objModelFilePath);

    void Update(Roose::Timestep deltaTime) override;
    void FixedUpdate(Roose::Timestep fixedDeltaTime) override;

    void Render() const;

    void SetMaterial(const Roose::Ref<Roose::Material>& material) { m_Material = material; }

    void ApplyForce(const glm::vec3& force) { RigidBody.ApplyForce(force); }
    [[nodiscard]] float GetRadius() const { return m_Radius; }

    [[nodiscard]] const Roose::Ref<Roose::RenderableObject>& GetRenderableObject() const { return m_RenderableObject; }

    RigidBodyComponent RigidBody;
private:
    Roose::Ref<Roose::RenderableObject> m_RenderableObject;
    Roose::Ref<Roose::Material> m_Material = nullptr;

    float m_Radius = 1.0f; // (Approximation) This is used as the radius of the sphere collider
};
