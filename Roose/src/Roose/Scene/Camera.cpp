#include "rspch.h"
#include "Roose/Scene/Camera.h"

namespace Roose {

    void Camera::SetViewportSize(const uint32_t width, const uint32_t height)
    {
        m_AspectRatio = static_cast<float>(width) / static_cast<float>(height);
        RecalculateProjection();
    }

}
