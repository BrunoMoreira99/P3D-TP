#include "rspch.h"
#include "Roose/Scene/Camera.h"

namespace Roose {

    void Camera::SetViewportSize(const uint32_t width, const uint32_t height)
    {
        if (width == 0 || height == 0) return;
        m_AspectRatio = static_cast<float>(width) / static_cast<float>(height);
        RecalculateProjection();
    }

}
