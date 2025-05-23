#pragma once

#include "GameObject.h"
#include "../Lighting/BaseLight.h"

class LightSourceGameObject : public GameObject
{
public:
    LightSourceGameObject(const Roose::Ref<BaseLight>& light)
        : m_Light(light) {}

    void Update(const Roose::Timestep deltaTime) override
    {
        UpdateLightTransform();
        GameObject::Update(deltaTime);
    }

    void SetLight(const Roose::Ref<BaseLight>& light)
    {
        m_Light = light;
        UpdateLightTransform();
    }

    [[nodiscard]] const Roose::Ref<BaseLight>& GetLight() const { return m_Light; }
private:
    void UpdateLightTransform() const
    {
        if (!m_Light) return;
        const auto worldTransform = GetWorldTransform();
        m_Light->SetPosition(worldTransform.Translation);
        m_Light->SetDirection(glm::normalize(worldTransform.Rotation * glm::vec3(0, -1, 0)));
    }
protected:
    Roose::Ref<BaseLight> m_Light = nullptr;
};
