#pragma once

#include <glm/glm.hpp>

namespace Roose {

    class Camera
    {
    public:
        Camera() = default;
        Camera(const glm::mat4& projection) : m_Projection(projection) {}
        virtual ~Camera() = default;

        virtual void SetViewportSize(uint32_t width, uint32_t height);

        [[nodiscard]] float GetNearClip() const { return m_NearClip; }
        void SetNearClip(const float nearClip) { m_NearClip = nearClip; RecalculateProjection(); }

        [[nodiscard]] float GetFarClip() const { return m_FarClip; }
        void SetFarClip(const float farClip) { m_FarClip = farClip; RecalculateProjection(); }

        [[nodiscard]] const glm::mat4& GetProjection() const { return m_Projection; }
        [[nodiscard]] float GetAspectRatio() const { return m_AspectRatio; }
    protected:
        Camera(const float nearClip, const float farClip) : m_NearClip(nearClip), m_FarClip(farClip) {}
        virtual void RecalculateProjection() {}
    protected:
        glm::mat4 m_Projection = glm::mat4(1.0f);
        float m_NearClip = 0.01f, m_FarClip = 1000.0f;
        float m_AspectRatio = 0.0f;
    };

}
