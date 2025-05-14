#pragma once

#include "BaseLight.h"

class AmbientLight : public BaseLight {
public:
    AmbientLight(const bool enabled = true, const glm::vec3& color = {1.0f, 1.0f, 1.0f})
        : BaseLight(enabled, color) {}

    void ApplyUniforms(const Roose::Ref<Roose::Shader>& shader, const std::string& uniformPrefix) override {
        if (m_IsDirty) {
            shader->SetBool(uniformPrefix + ".enabled", m_Enabled);
            shader->SetFloat3(uniformPrefix + ".color", m_Color);
            m_IsDirty = false;
        }
    }
};
