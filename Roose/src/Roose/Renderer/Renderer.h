#pragma once

#include <glm/glm.hpp>

namespace Roose {

    class Renderer
	{
	public:
    	static void Init();
    	static void Shutdown();

    	static void OnWindowResize(uint32_t width, uint32_t height);
    	static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

    	static void SetClearColor(const glm::vec4& color);
    	static void Clear();
	};

}
