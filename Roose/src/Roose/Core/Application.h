#pragma once

#include "Roose/Core/Base.h"
#include "Roose/Core/Window.h"
#include "Roose/Core/LayerStack.h"
#include "Roose/Events/ApplicationEvent.h"

int main(int argc, char** argv);

namespace Roose {

    struct ApplicationCommandLineArgs
    {
        int Count = 0;
        char** Args = nullptr;

        const char* operator[](const int index) const
        {
            RS_ASSERT(index < Count)
            return Args[index];
        }
    };

    struct ApplicationSpecification
    {
        std::string Name = "Roose Application";
        std::string WorkingDirectory;
        ApplicationCommandLineArgs CommandLineArgs;
    };

    class Application
    {
    public:
        Application(const ApplicationSpecification& specification);
        virtual ~Application();

        void OnEvent(Event& e);

        void PushLayer(Layer* layer);

        [[nodiscard]] Window& GetWindow() const { return *m_Window; }

        void Close();

        static Application& Get() { return *s_Instance; }

        [[nodiscard]] const ApplicationSpecification& GetSpecification() const { return m_Specification; }
    private:
        void Run();
        bool OnWindowClose(WindowCloseEvent& e);
        bool OnWindowResize(WindowResizeEvent& e);
    private:
        ApplicationSpecification m_Specification;
        Scope<Window> m_Window;
        bool m_Running = true;
        bool m_Minimized = false;
        LayerStack m_LayerStack;
        float m_LastFrameTime = 0.0f;

        std::vector<std::function<void()>> m_MainThreadQueue;
    private:
        static Application* s_Instance;
        friend int ::main(int argc, char** argv);
    };

    // To be defined in CLIENT
    Application* CreateApplication(ApplicationCommandLineArgs args);

}
