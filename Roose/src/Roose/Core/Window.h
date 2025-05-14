#pragma once

#include "Roose/Renderer/GraphicsContext.h"
#include "Roose/Events/Event.h"

struct GLFWwindow;

namespace Roose {

    enum class WindowCursorMode : int32_t
    {
        // From GLFW
        Normal   = 0x34001, // Makes the cursor visible and behaving normally.
        Hidden   = 0x34002, // Makes the cursor invisible when it is over the content area of the window but does not restrict the cursor from leaving.
        Disabled = 0x34003, // Hides and grabs the cursor, providing virtual and unlimited cursor movement.
        Captured = 0x34004  // Makes the cursor visible and confines it to the content area of the window.
    };

    struct WindowProps
    {
        std::string Title;
        uint32_t Width;
        uint32_t Height;

        WindowProps(
            const std::string& title = "Roose",
            uint32_t width = 1600,
            uint32_t height = 900
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

        void SetTitle(const std::string& title);
        void SetEventCallback(const EventCallbackFn& callback) { m_Data.EventCallback = callback; }
        void SetVSync(bool enabled);
        void SetCursorMode(WindowCursorMode mode);
        void ToggleFullscreen();

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
            bool Fullscreen = false;
            WindowCursorMode CursorMode;

            EventCallbackFn EventCallback;
        };

        WindowData m_Data;

        // Saved data for restoring window size and position after returning from fullscreen
        int32_t m_SavedWindowWidth, m_SavedWindowHeight, m_SavedWindowX, m_SavedWindowY;
    };

}
