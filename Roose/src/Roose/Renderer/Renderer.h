#pragma once

#include "Roose/Renderer/Camera.h"
#include "Roose/Renderer/Material.h"
#include "Roose/Renderer/Mesh.h"
#include "Roose/Renderer/Texture2D.h"
#include "Roose/Renderer/VertexArray.h"

#include <glm/glm.hpp>

namespace Roose {

    class [[deprecated("Not implemented yet and out of scope for this 3D Programming project.")]] Renderer
	{
	public:
    	static void Init();
    	static void Shutdown();

    	static void OnWindowResize(uint32_t width, uint32_t height);
    	static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);

    	static void SetClearColor(const glm::vec4& color);
    	static void Clear();

		static void BeginScene(const Camera& camera, const glm::mat4& transform);
		static void EndScene();
		static void Flush();

    	static void Submit(const Ref<Mesh>& mesh, const Ref<Material>& material, const glm::mat4& transform = glm::mat4(1.0f));

		// Primitives
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
		static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture);
		static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture);
    	static void DrawQuad(const glm::mat4& transform, const glm::vec4& color);
    	static void DrawQuad(const glm::mat4& transform, const Ref<Texture2D>& texture);

    	static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0);
	private:
		static void StartBatch();
		static void NextBatch();
	};

}
