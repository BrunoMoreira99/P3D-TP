#pragma once
#include "Roose/Scene/Camera.h"

namespace Roose {

    class OrthographicCamera final : public Camera
    {
    public:
        OrthographicCamera()
        {
            RecalculateProjection();
        }
        OrthographicCamera(const float size, const float nearClip, const float farClip)
            : Camera(nearClip, farClip), m_Size(size)
        {
            RecalculateProjection();
        }

        [[nodiscard]] float GetSize() const { return m_Size; }
        void SetSize(const float size) { m_Size = size; RecalculateProjection(); }
    private:
        void RecalculateProjection() override;
    private:
        float m_Size = 10.0f;
    };

}
