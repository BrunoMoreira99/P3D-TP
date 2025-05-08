#include "rspch.h"
#include "Roose/Renderer/IndexBuffer.h"

#include <GL/glew.h>

namespace Roose {

    IndexBuffer::IndexBuffer(const uint32_t* indices, const uint32_t count) : m_Count(count)
    {
        glCreateBuffers(1, &m_RendererID);
        glNamedBufferData(m_RendererID, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
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

}
