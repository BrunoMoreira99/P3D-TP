#include "rspch.h"
#include "Roose/Core/Time.h"

#include <GLFW/glfw3.h>

namespace Roose {

    float Time::GetTime()
    {
        return static_cast<float>(glfwGetTime());
    }

}
