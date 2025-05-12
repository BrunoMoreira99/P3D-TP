#include "rspch.h"
#include "Roose/Scene/PerspectiveCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Roose {

    void PerspectiveCamera::RecalculateProjection()
    {
        m_Projection = glm::perspective(m_VerticalFOV, m_AspectRatio, m_NearClip, m_FarClip);
    }

}
