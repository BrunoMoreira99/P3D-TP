#pragma once

#include "Roose/Renderer/VertexBuffer.h"
#include "Roose/Renderer/IndexBuffer.h"

namespace Roose {

    class VertexArray
    {
    public:
        VertexArray();
        ~VertexArray();

        void Bind() const;
        void Unbind() const;

        void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer);
        void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer);

        [[nodiscard]] const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const { return m_VertexBuffers; }
        [[nodiscard]] const Ref<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffer; }

        static Ref<VertexArray> Create();
    private:
        uint32_t m_RendererID;
        uint32_t m_VertexBufferIndex = 0;
        std::vector<Ref<VertexBuffer>> m_VertexBuffers;
        Ref<IndexBuffer> m_IndexBuffer;
    };

}
