#include "rspch.h"
#include "Roose/Renderer/VertexBuffer.h"

namespace Roose {

    VertexBuffer::VertexBuffer(const float* vertices, const GLsizeiptr size)
    {
        glCreateBuffers(1, &m_RendererID);
        // With Direct State Access (DSA), we can create and populate buffer objects directly, without needing to bind them first.
        // The target of the buffer object is not required during the creation or data population process.
        // (See: https://www.khronos.org/opengl/wiki/Buffer_Object)
        //  > "When you're just creating, filling the buffer object with data, or both, the target you use doesn't technically matter."
        // Only when the buffer is bound for actual use does the target become relevant.
        glNamedBufferData(m_RendererID, size, vertices, GL_STATIC_DRAW);
    }

    VertexBuffer::~VertexBuffer()
    {
        glDeleteBuffers(1, &m_RendererID);
    }

    void VertexBuffer::Bind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
    }

    void VertexBuffer::Unbind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void VertexBuffer::SetData(const void* data, const uint32_t size)
    {
        glNamedBufferSubData(m_RendererID, 0, size, data);
    }

}
