#include "rspch.h"
#include "Roose/Renderer/GraphicsContext.h"

#include <Gl/glew.h>
#include <GLFW/glfw3.h>

namespace Roose {

	Scope<GraphicsContext> GraphicsContext::Create(GLFWwindow* window)
	{
		return CreateScope<GraphicsContext>(window);
	}

	GraphicsContext::GraphicsContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		RS_ASSERT(windowHandle, "Window handle is null!")
	}

	void GraphicsContext::Init() const
	{
		glfwMakeContextCurrent(m_WindowHandle);
		const GLenum status = glewInit();
		RS_ASSERT(status == GLEW_OK, "Failed to initialize GLEW!")

		RS_INFO("OpenGL Info:");
		RS_INFO("  Vendor: %s", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
		RS_INFO("  Renderer: %s", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
		RS_INFO("  Version: %s", reinterpret_cast<const char*>(glGetString(GL_VERSION)));

		GLint glMajor, glMinor;
		glGetIntegerv(GL_MAJOR_VERSION, &glMajor);
		glGetIntegerv(GL_MINOR_VERSION, &glMinor);

		RS_ASSERT(glMajor > 4 || (glMajor == 4 && glMinor >= 5), "Roose requires at least OpenGL version 4.5!")
	}

	void GraphicsContext::SwapBuffers() const
	{
		glfwSwapBuffers(m_WindowHandle);
	}

}
