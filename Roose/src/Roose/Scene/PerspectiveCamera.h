#pragma once
#include "Roose/Scene/Camera.h"

namespace Roose {

    class PerspectiveCamera final : public Camera
    {
    public:
        PerspectiveCamera()
        {
            RecalculateProjection();
        }
        PerspectiveCamera(const float verticalFOV, const float nearClip, const float farClip)
            : Camera(nearClip, farClip), m_VerticalFOV(verticalFOV)
        {
            RecalculateProjection();
        }

        [[nodiscard]] float GetVerticalFOV() const { return m_VerticalFOV; }
        void SetVerticalFOV(const float size) { m_VerticalFOV = size; RecalculateProjection(); }
    private:
        void RecalculateProjection() override;
    private:
        float m_VerticalFOV = glm::radians(45.0f);
    };

}
