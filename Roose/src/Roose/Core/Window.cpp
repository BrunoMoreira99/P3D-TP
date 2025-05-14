#include "rspch.h"

#include "Roose/Core/Window.h"

#include "Roose/Events/ApplicationEvent.h"
#include "Roose/Events/MouseEvent.h"
#include "Roose/Events/KeyEvent.h"

#include <GLFW/glfw3.h>

namespace Roose {

    static uint8_t s_GLFWWindowCount = 0;

    Scope<Window> Window::Create(const WindowProps& props)
    {
        return CreateScope<Window>(props);
    }

    static void GLFWErrorCallback(const int32_t error, const char* description)
    {
        RS_ERROR("GLFW Error (%d): %s", error, description);
    }

    Window::Window(const WindowProps& props)
    {
        Init(props);
    }

    Window::~Window()
    {
        Shutdown();
    }

    void Window::Init(const WindowProps& props)
    {
        m_Data.Title = props.Title;
        m_Data.Width = props.Width;
        m_Data.Height = props.Height;

        RS_INFO("Creating window %s (%u, %u)", props.Title.c_str(), props.Width, props.Height);

        if (s_GLFWWindowCount == 0)
        {
            const int32_t success = glfwInit();
            RS_ASSERT(success, "Could not initialize GLFW!")
            glfwSetErrorCallback(GLFWErrorCallback);
        }

        #ifdef RS_DEBUG
            glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
        #endif
        m_Window = glfwCreateWindow(static_cast<int32_t>(props.Width), static_cast<int32_t>(props.Height), m_Data.Title.c_str(), nullptr, nullptr);
        ++s_GLFWWindowCount;

        m_Context = GraphicsContext::Create(m_Window);
        m_Context->Init();

        glfwSetWindowUserPointer(m_Window, &m_Data);
        SetVSync(true);

        // Set GLFW callbacks
        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, const int32_t width, const int32_t height)
        {
            // If both width and height are 0, ignore the event (this happens when the window is minimized)
            if (width == 0 && height == 0) return;

            WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
            data.Width = width;
            data.Height = height;

            WindowResizeEvent event(width, height);
            data.EventCallback(event);
        });

        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
        {
            const WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));
            WindowCloseEvent event;
            data.EventCallback(event);
        });

        glfwSetKeyCallback(m_Window, [](GLFWwindow* window, const int32_t key, const int32_t scancode, const int32_t action, const int32_t mods)
        {
            const WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

            switch (action)
            {
                case GLFW_PRESS:
                {
                    KeyDownEvent event(key, false);
                    data.EventCallback(event);
                    break;
                }
                case GLFW_RELEASE:
                {
                    KeyUpEvent event(key);
                    data.EventCallback(event);
                    break;
                }
                case GLFW_REPEAT:
                {
                    KeyDownEvent event(key, true);
                    data.EventCallback(event);
                    break;
                }
            }
        });

        glfwSetCharCallback(m_Window, [](GLFWwindow* window, const uint32_t keycode)
        {
            const WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

            KeyPressEvent event(keycode);
            data.EventCallback(event);
        });

        glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, const int32_t button, const int32_t action, const int32_t mods)
        {
            const WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

            switch (action)
            {
                case GLFW_PRESS:
                {
                    MouseButtonPressEvent event(button);
                    data.EventCallback(event);
                    break;
                }
                case GLFW_RELEASE:
                {
                    MouseButtonReleaseEvent event(button);
                    data.EventCallback(event);
                    break;
                }
            }
        });

        glfwSetScrollCallback(m_Window, [](GLFWwindow* window, const double xOffset, const double yOffset)
        {
            const WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

            MouseScrollEvent event(static_cast<float>(xOffset), static_cast<float>(yOffset));
            data.EventCallback(event);
        });

        glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, const double xPos, const double yPos)
        {
            const WindowData& data = *static_cast<WindowData*>(glfwGetWindowUserPointer(window));

            MouseMoveEvent event(static_cast<float>(xPos), static_cast<float>(yPos));
            data.EventCallback(event);
        });
    }

    void Window::Shutdown()
    {
        glfwDestroyWindow(m_Window);
        --s_GLFWWindowCount;

        if (s_GLFWWindowCount == 0)
        {
            glfwTerminate();
        }
    }

    void Window::OnUpdate()
    {
        glfwPollEvents();
        m_Context->SwapBuffers();
    }

    void Window::SetTitle(const std::string& title)
    {
        glfwSetWindowTitle(m_Window, title.c_str());
        m_Data.Title = title;
    }

    void Window::ToggleFullscreen()
    {
        if (!m_Data.Fullscreen)
        {
            m_SavedWindowWidth = static_cast<int32_t>(m_Data.Width);
            m_SavedWindowHeight = static_cast<int32_t>(m_Data.Height);
            glfwGetWindowPos(m_Window, &m_SavedWindowX, &m_SavedWindowY);
            const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
            glfwSetWindowMonitor(m_Window, glfwGetPrimaryMonitor(), 0, 0,
                mode->width, mode->height, mode->refreshRate);
            m_Data.Width = mode->width;
            m_Data.Height = mode->height;
            m_Data.Fullscreen = true;
        }
        else
        {
            glfwSetWindowMonitor(m_Window, nullptr, m_SavedWindowX, m_SavedWindowY,
                m_SavedWindowWidth, m_SavedWindowHeight, 0);
            m_Data.Fullscreen = false;
        }
    }

    void Window::SetVSync(const bool enabled)
    {
        glfwSwapInterval(enabled ? 1 : 0);
        m_Data.VSync = enabled;
    }

    void Window::SetCursorMode(const WindowCursorMode mode)
    {
        glfwSetInputMode(m_Window, GLFW_CURSOR, static_cast<int32_t>(mode));
        m_Data.CursorMode = mode;
    }

    bool Window::IsVSync() const
    {
        return m_Data.VSync;
    }

}
