#include "rspch.h"
#include "Roose/Renderer/Renderer.h"
#include "Roose/Renderer/Shader.h"
#include "Roose/Renderer/VertexArray.h"
#include "Roose/Shaders/ScreenSpaceUnlit2D.cpp"

#include <GL/glew.h>
#include "glm/gtx/transform.hpp"

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

    struct RendererData
    {
        Ref<UniformBuffer> CameraUniformBuffer;
        Ref<UniformBuffer> ModelUniformBuffer;
        Ref<UniformBuffer> MaterialDataUniformBuffer;

        Ref<VertexArray> VertexArray;
        Ref<VertexBuffer> VertexBuffer;
        Ref<Shader> ScreenSpaceUnlit2DShader;
    };

    static RendererData s_Data;

    void Renderer::Init()
    {
    #ifdef RS_DEBUG
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(OpenGLMessageCallback, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
    #endif
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_DEPTH_TEST);

        // Create Uniform Buffers - 1024 bytes (1KB) for each should be generous enough
        s_Data.CameraUniformBuffer       = UniformBuffer::Create(1024, 0);
        s_Data.ModelUniformBuffer        = UniformBuffer::Create(1024, 1);
        s_Data.MaterialDataUniformBuffer = UniformBuffer::Create(1024, 2);

        // Create Vertex Array
        s_Data.VertexArray = VertexArray::Create();

        // Create Vertex Buffer
        constexpr float vertices[] = {
            // Positions   // TexCoords
            -0.5f, -0.5f,  0.0f, 1.0f,  // Bottom Left
             0.5f, -0.5f,  1.0f, 1.0f,  // Bottom Right
             0.5f,  0.5f,  1.0f, 0.0f,  // Top Right
            -0.5f,  0.5f,  0.0f, 0.0f   // Top Left
        };
        const Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
        vertexBuffer->SetLayout({
            { ShaderDataType::Float2, "a_Position" },
            { ShaderDataType::Float2, "a_TexCoord" }
        });
        s_Data.VertexArray->AddVertexBuffer(vertexBuffer);

        // Create Index Buffer
        constexpr uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };
        const Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(indices, 6);
        s_Data.VertexArray->SetIndexBuffer(indexBuffer);

        s_Data.ScreenSpaceUnlit2DShader = Shader::FromGLSLString("ScreenSpaceUnlit2D", ScreenSpaceUnlit2DShaderSource);
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
        s_Data.ScreenSpaceUnlit2DShader->SetMat4("u_OrthoProjection",
            glm::ortho(
                0.0f, static_cast<float>(width),
                static_cast<float>(height), 0.0f,
                -1.0f, 1.0f
            )
        );
    }

    void Renderer::SetClearColor(const glm::vec4& color)
    {
        glClearColor(color.r, color.g, color.b, color.a);
    }

    void Renderer::Clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Renderer::ClearColor()
    {
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void Renderer::ClearDepth()
    {
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    void Renderer::DrawIndexed(const Ref<VertexArray>& vertexArray, const uint32_t indexCount)
    {
        vertexArray->Bind();
        const uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(count), GL_UNSIGNED_INT, nullptr);
    }

    void Renderer::DrawQuad(const float x, const float y, const float width, const float height) {
        // Convert position and size to normalized device coordinates
        const glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f))
            * glm::scale(glm::mat4(1.0f), glm::vec3(width, height, 1.0f));

        s_Data.ScreenSpaceUnlit2DShader->Bind();
        s_Data.ScreenSpaceUnlit2DShader->SetMat4("u_Transform", transform);
        DrawIndexed(s_Data.VertexArray);
    }

    Ref<UniformBuffer> Renderer::GetCameraUniformBuffer()
    {
        return s_Data.CameraUniformBuffer;
    }

    Ref<UniformBuffer> Renderer::GetModelUniformBuffer()
    {
        return s_Data.ModelUniformBuffer;
    }

    Ref<UniformBuffer> Renderer::GetMaterialDataUniformBuffer()
    {
        return s_Data.MaterialDataUniformBuffer;
    }

}
