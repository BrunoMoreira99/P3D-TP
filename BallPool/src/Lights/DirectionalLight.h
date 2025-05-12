#pragma once

#include "BaseLight.h"

class DirectionalLight : public BaseLight {
public:
    DirectionalLight(
        const bool enabled = true, const glm::vec3& color = {1.0f, 1.0f, 1.0f},
        const glm::vec3& direction = {-0.5f, -1.0f, -0.5f}
    ) : BaseLight(enabled, color), m_Direction(direction) {}

    void SetDirection(const glm::vec3& direction) {
        if (m_Direction != direction) {
            m_Direction = direction;
            m_IsDirty = true;
        }
    }

    void ApplyUniforms(const Roose::Ref<Roose::Shader>& shader, const std::string& uniformPrefix) override {
        if (m_IsDirty) {
            shader->SetBool(uniformPrefix + ".enabled", m_Enabled);
            shader->SetFloat3(uniformPrefix + ".color", m_Color);
            shader->SetFloat3(uniformPrefix + ".direction", m_Direction);
            m_IsDirty = false;
        }
    }

private:
    glm::vec3 m_Direction;
};
