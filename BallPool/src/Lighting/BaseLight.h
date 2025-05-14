#pragma once

#include <Roose/Renderer/Shader.h>

#include <glm/glm.hpp>
#include <string>

class BaseLight {
public:
    BaseLight(const bool enabled = true, const glm::vec3& color = {1.0f, 1.0f, 1.0f})
        : m_Enabled(enabled), m_Color(color), m_IsDirty(true) {}

    virtual ~BaseLight() = default;

    void Toggle() {
        m_Enabled = !m_Enabled;
        m_IsDirty = true;
    }

    void SetEnabled(const bool enabled) {
        if (m_Enabled != enabled) {
            m_Enabled = enabled;
            m_IsDirty = true;
        }
    }

    void SetColor(const glm::vec3& color) {
        if (m_Color != color) {
            m_Color = color;
            m_IsDirty = true;
        }
    }

    [[nodiscard]] bool IsDirty() const { return m_IsDirty; }

    virtual void ApplyUniforms(const Roose::Ref<Roose::Shader>& shader, const std::string& uniformPrefix) = 0;
protected:
    bool m_Enabled;
    glm::vec3 m_Color;
    bool m_IsDirty;
};
