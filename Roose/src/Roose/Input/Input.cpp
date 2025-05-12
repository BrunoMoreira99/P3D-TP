#include "rspch.h"

#include "Roose/Input/Input.h"
#include "Roose/Core/Application.h"

#include <GLFW/glfw3.h>

namespace Roose {

    bool Input::IsKeyDown(const KeyCode key)
    {
        GLFWwindow* window = Application::Get().GetWindow().GetNativeWindow();
        const auto state = glfwGetKey(window, key);
        return state == GLFW_PRESS;
    }

    bool Input::IsMouseButtonPress(const MouseCode button)
    {
        GLFWwindow* window = Application::Get().GetWindow().GetNativeWindow();
        const auto state = glfwGetMouseButton(window, button);
        return state == GLFW_PRESS;
    }

    glm::vec2 Input::GetMousePosition()
    {
        GLFWwindow* window = Application::Get().GetWindow().GetNativeWindow();
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        return { static_cast<float>(xpos), static_cast<float>(ypos) };
    }

    float Input::GetMouseX()
    {
        return GetMousePosition().x;
    }

    float Input::GetMouseY()
    {
        return GetMousePosition().y;
    }

}
