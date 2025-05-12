#include "rspch.h"
#include "Roose/Renderer/Renderer.h"
#include "Roose/Renderer/Shader.h"

#include <GL/glew.h>

namespace Roose {

    static void OpenGLMessageCallback(
        const uint32_t source,
        const uint32_t type,
        const uint32_t id,
        const uint32_t severity,
        const int32_t length,
        const char* message,
        const void* userParam)
    {
        switch (severity)
        {
            case GL_DEBUG_SEVERITY_HIGH:         RS_ERROR("%s", message); return;
            case GL_DEBUG_SEVERITY_MEDIUM:       RS_ERROR("%s", message); return;
            case GL_DEBUG_SEVERITY_LOW:          RS_INFO("%s", message);  return;
            case GL_DEBUG_SEVERITY_NOTIFICATION: RS_INFO("%s", message);  return;
        }

        RS_ASSERT(false, "Unknown severity level!")
    }

    void Renderer::Init()
    {
	#ifdef RS_DEBUG
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(OpenGLMessageCallback, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_TRUE);
	#endif
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_DEPTH_TEST);

        // Load shaders
        ShaderLibrary::Load("assets/shaders/BlinnPhong.glsl");
    }

    void Renderer::Shutdown()
    {
    }

    void Renderer::OnWindowResize(const uint32_t width, const uint32_t height)
    {
        SetViewport(0, 0, width, height);
    }

    void Renderer::SetViewport(const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height)
    {
        glViewport(static_cast<GLint>(x), static_cast<GLint>(y), static_cast<GLsizei>(width), static_cast<GLsizei>(height));
    }

    void Renderer::SetClearColor(const glm::vec4& color)
    {
        glClearColor(color.r, color.g, color.b, color.a);
    }

    void Renderer::Clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

}
