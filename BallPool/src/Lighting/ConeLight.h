#pragma once

#include "BaseLight.h"

class ConeLight : public BaseLight {
public:
    ConeLight(
        const bool enabled = true, const glm::vec3& color = {1.0f, 1.0f, 1.0f},
        const glm::vec3& position = {2.0f, 3.0f, 2.0f},
        const glm::vec3& direction = {-1.0f, -1.0f, -1.0f},
        const float angle = glm::cos(glm::radians(30.0f))
    ) : BaseLight(enabled, color), m_Position(position), m_Direction(direction), m_Angle(angle) {}

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

    void SetAngle(const float angle) {
        if (m_Angle != angle) {
            m_Angle = angle;
            m_IsDirty = true;
        }
    }

    void ApplyUniforms(const Roose::Ref<Roose::Shader>& shader, const std::string& uniformPrefix) override {
        if (m_IsDirty) {
            shader->SetBool(uniformPrefix + ".enabled", m_Enabled);
            shader->SetFloat3(uniformPrefix + ".color", m_Color);
            shader->SetFloat3(uniformPrefix + ".position", m_Position);
            shader->SetFloat3(uniformPrefix + ".direction", m_Direction);
            shader->SetFloat(uniformPrefix + ".angle", m_Angle);
            m_IsDirty = false;
        }
    }

private:
    glm::vec3 m_Position;
    glm::vec3 m_Direction;
    float m_Angle;
};
