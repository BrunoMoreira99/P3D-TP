#pragma once

struct GLFWwindow;

namespace Roose {

	class GraphicsContext
	{
	public:
		GraphicsContext(GLFWwindow* windowHandle);

		void Init();
		void SwapBuffers();

		static Scope<GraphicsContext> Create(GLFWwindow* window);
	private:
		GLFWwindow* m_WindowHandle;
	};

}
