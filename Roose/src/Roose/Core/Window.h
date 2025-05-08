#pragma once

#include "Roose/Renderer/GraphicsContext.h"
#include "Roose/Events/Event.h"

#include <GLFW/glfw3.h>

namespace Roose {

    struct WindowProps
    {
        std::string Title;
        uint32_t Width;
        uint32_t Height;

        WindowProps(
            const std::string& title = "Roose",
            uint32_t width = 1280,
            uint32_t height = 720
        ) : Title(title), Width(width), Height(height) {}
    };

    class Window
    {
    public:
        using EventCallbackFn = std::function<void(Event&)>;

        Window(const WindowProps& props);
        ~Window();

        void OnUpdate();

        [[nodiscard]] uint32_t GetWidth() const { return m_Data.Width; }
        [[nodiscard]] uint32_t GetHeight() const { return m_Data.Height; }

        // Window attributes
        void SetEventCallback(const EventCallbackFn& callback) { m_Data.EventCallback = callback; }
        void SetVSync(bool enabled);
        [[nodiscard]] bool IsVSync() const;

        [[nodiscard]] GLFWwindow* GetNativeWindow() const { return m_Window; }

        static Scope<Window> Create(const WindowProps& props = WindowProps());
    private:
        void Init(const WindowProps& props);
        void Shutdown();
    private:
        GLFWwindow* m_Window;
        Scope<GraphicsContext> m_Context;

        struct WindowData
        {
            std::string Title;
            uint32_t Width, Height;
            bool VSync;

            EventCallbackFn EventCallback;
        };

        WindowData m_Data;
    };

}
