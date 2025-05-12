#pragma once

struct GLFWwindow;

namespace Roose {

	class GraphicsContext
	{
	public:
		GraphicsContext(GLFWwindow* windowHandle);

		void Init() const;
		void SwapBuffers() const;

		static Scope<GraphicsContext> Create(GLFWwindow* window);
	private:
		GLFWwindow* m_WindowHandle;
	};

}
