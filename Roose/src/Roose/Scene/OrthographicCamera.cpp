#include "rspch.h"
#include "Roose/Scene/OrthographicCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Roose {

    void OrthographicCamera::RecalculateProjection()
    {
        const float orthoLeft = -m_Size * m_AspectRatio * 0.5f;
        const float orthoRight = m_Size * m_AspectRatio * 0.5f;
        const float orthoBottom = -m_Size * 0.5f;
        const float orthoTop = m_Size * 0.5f;

        m_Projection = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, m_NearClip, m_FarClip);
    }

}
