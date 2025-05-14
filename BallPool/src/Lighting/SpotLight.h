#pragma once

#include "BaseLight.h"

class SpotLight final : public BaseLight {
public:
    SpotLight(
        const bool enabled = true, const glm::vec3& color = {1.0f, 1.0f, 1.0f},
        const glm::vec3& position = {0.0f, 5.0f, 0.0f},
        const glm::vec3& direction = {0.0f, -1.0f, 0.0f},
        const float cutoff = glm::cos(glm::radians(12.5f)),
        const float outerCutoff = glm::cos(glm::radians(17.5f))
    ) : BaseLight(enabled, color), m_Position(position), m_Direction(direction), m_Cutoff(cutoff), m_OuterCutoff(outerCutoff) {}

    void SetPosition(const glm::vec3& position) {
        if (m_Position != position) {
            m_Position = position;
            m_IsDirty = true;
        }
    }

    void SetDirection(const glm::vec3& direction) {
        if (m_Direction != direction) {
            m_Direction = direction;
            m_IsDirty = true;
        }
    }

    void SetCutoff(const float cutoff) {
        if (m_Cutoff != cutoff) {
            m_Cutoff = cutoff;
            m_IsDirty = true;
        }
    }

    void SetOuterCutoff(const float outerCutoff) {
        if (m_OuterCutoff != outerCutoff) {
            m_OuterCutoff = outerCutoff;
            m_IsDirty = true;
        }
    }

    void ApplyUniforms(const Roose::Ref<Roose::Shader>& shader, const std::string& uniformPrefix) override {
        if (m_IsDirty) {
            shader->SetBool(uniformPrefix + ".enabled", m_Enabled);
            shader->SetFloat3(uniformPrefix + ".color", m_Color);
            shader->SetFloat3(uniformPrefix + ".position", m_Position);
            shader->SetFloat3(uniformPrefix + ".direction", m_Direction);
            shader->SetFloat(uniformPrefix + ".cutoff", m_Cutoff);
            shader->SetFloat(uniformPrefix + ".outerCutoff", m_OuterCutoff);
            m_IsDirty = false;
        }
    }

private:
    glm::vec3 m_Position;
    glm::vec3 m_Direction;
    float m_Cutoff;
    float m_OuterCutoff;
};
