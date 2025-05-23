#pragma once

#include "Roose/Renderer/UniformBuffer.h"
#include "Roose/Renderer/VertexArray.h"

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
        static void ClearColor();
        static void ClearDepth();
        static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0);

        /**
         * @brief Draw a quad at the specified position and size.
         * @param x The x-coordinate of the quad's position in pixels.
         * @param y The y-coordinate of the quad's position in pixels.
         * @param width The width of the quad in pixels.
         * @param height The height of the quad in pixels.
         */
        static void DrawQuad(float x, float y, float width, float height);

        static Ref<UniformBuffer> GetCameraUniformBuffer();
        static Ref<UniformBuffer> GetModelUniformBuffer();
        static Ref<UniformBuffer> GetMaterialDataUniformBuffer();
    };

}
