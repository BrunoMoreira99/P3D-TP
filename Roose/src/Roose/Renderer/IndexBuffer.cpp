#include "rspch.h"
#include "Roose/Renderer/IndexBuffer.h"

#include <GL/glew.h>

namespace Roose {

    Ref<IndexBuffer> IndexBuffer::Create(const uint32_t count)
    {
        return CreateRef<IndexBuffer>(count);
    }

    Ref<IndexBuffer> IndexBuffer::Create(const uint32_t* indices, const uint32_t count)
    {
        return CreateRef<IndexBuffer>(indices, count);
    }

    IndexBuffer::IndexBuffer(const uint32_t count) : m_Count(count)
    {
        glCreateBuffers(1, &m_RendererID);
        glNamedBufferData(m_RendererID, static_cast<GLsizeiptr>(count * sizeof(uint32_t)), nullptr, GL_DYNAMIC_DRAW);
    }

    IndexBuffer::IndexBuffer(const uint32_t* indices, const uint32_t count) : m_Count(count)
    {
        glCreateBuffers(1, &m_RendererID);
        glNamedBufferData(m_RendererID, static_cast<GLsizeiptr>(count * sizeof(uint32_t)), indices, GL_STATIC_DRAW);
    }

    IndexBuffer::~IndexBuffer()
    {
        glDeleteBuffers(1, &m_RendererID);
    }

    void IndexBuffer::Bind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
    }

    void IndexBuffer::Unbind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void IndexBuffer::SetData(const uint32_t* indices, const uint32_t count, const uint32_t offset)
    {
        RS_ASSERT((offset + count) * sizeof(uint32_t) <= m_Count * sizeof(uint32_t), 
                  "IndexBuffer::SetData: offset + count exceeds buffer size!")

        glNamedBufferSubData(
            m_RendererID,
            static_cast<GLintptr>(offset * sizeof(uint32_t)),
            static_cast<GLsizeiptr>(count * sizeof(uint32_t)),
            indices
        );
    }

}
