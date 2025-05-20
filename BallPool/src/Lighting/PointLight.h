#pragma once

#include "BaseLight.h"

class PointLight : public BaseLight {
public:
    PointLight(
        const bool enabled = true, const glm::vec3& color = {1.0f, 1.0f, 1.0f},
        const glm::vec3& position = {0.0f, 3.0f, 0.0f},
        const float range = 20.0f
    ) : BaseLight(enabled, color), m_Position(position), m_Range(range) {}

    void SetPosition(const glm::vec3& position) {
        if (m_Position != position) {
            m_Position = position;
            m_IsDirty = true;
        }
    }

    void SetRange(const float range) {
        if (m_Range != range) {
            m_Range = range;
            m_IsDirty = true;
        }
    }

    void ApplyUniforms(const Roose::Ref<Roose::Shader>& shader, const std::string& uniformPrefix) override {
        if (m_IsDirty) {
            shader->SetBool(uniformPrefix + ".enabled", m_Enabled);
            shader->SetFloat3(uniformPrefix + ".color", m_Color);
            shader->SetFloat3(uniformPrefix + ".position", m_Position);
            shader->SetFloat(uniformPrefix + ".range", m_Range);
            m_IsDirty = false;
        }
    }

private:
    glm::vec3 m_Position;
    float m_Range;
};
