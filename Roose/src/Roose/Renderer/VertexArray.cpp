#include "rspch.h"
#include "Roose/Renderer/VertexArray.h"

#include <Gl/glew.h>

namespace Roose
{

    static GLenum ShaderDataTypeToOpenGLBaseType(const ShaderDataType type)
    {
        switch (type)
        {
            case ShaderDataType::Float:  return GL_FLOAT;
            case ShaderDataType::Float2: return GL_FLOAT;
            case ShaderDataType::Float3: return GL_FLOAT;
            case ShaderDataType::Float4: return GL_FLOAT;
            case ShaderDataType::Mat3:   return GL_FLOAT;
            case ShaderDataType::Mat4:   return GL_FLOAT;
            case ShaderDataType::Int:    return GL_INT;
            case ShaderDataType::Int2:   return GL_INT;
            case ShaderDataType::Int3:   return GL_INT;
            case ShaderDataType::Int4:   return GL_INT;
            case ShaderDataType::Bool:   return GL_BOOL;
        }

        RS_ASSERT(false, "Unknown ShaderDataType!")
        return 0;
    }

    Ref<VertexArray> VertexArray::Create()
    {
        return CreateRef<VertexArray>();
    }

    VertexArray::VertexArray()
    {
        glCreateVertexArrays(1, &m_RendererID);
    }

    VertexArray::~VertexArray()
    {
        glDeleteVertexArrays(1, &m_RendererID);
    }

    void VertexArray::Bind() const
    {
        glBindVertexArray(m_RendererID);
    }

    void VertexArray::Unbind() const
    {
        glBindVertexArray(0);
    }

    void VertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
    {
        RS_ASSERT(!vertexBuffer->GetLayout().GetElements().empty(), "Vertex Buffer has no layout!")

        const auto& layout = vertexBuffer->GetLayout();
        GLuint bindingIndex = static_cast<GLuint>(m_VertexBuffers.size());

        glVertexArrayVertexBuffer(
            m_RendererID,
            bindingIndex,
            vertexBuffer->GetRendererID(),
            0, // Offset in buffer
            static_cast<GLsizei>(layout.GetStride())
        );

        for (const auto& element : layout)
        {
            const GLuint attribIndex = m_VertexBufferIndex;
            const GLint componentCount = static_cast<GLint>(element.GetComponentCount());

            switch (element.Type)
            {
                case ShaderDataType::Float:
                case ShaderDataType::Float2:
                case ShaderDataType::Float3:
                case ShaderDataType::Float4:
                {
                    glEnableVertexArrayAttrib(m_RendererID, attribIndex);
                    glVertexArrayAttribFormat(
                        m_RendererID,
                        attribIndex,
                        componentCount,
                        ShaderDataTypeToOpenGLBaseType(element.Type),
                        element.Normalized ? GL_TRUE : GL_FALSE,
                        static_cast<GLuint>(element.Offset)
                    );
                    glVertexArrayAttribBinding(m_RendererID, attribIndex, bindingIndex);
                    m_VertexBufferIndex++;
                    break;
                }

                case ShaderDataType::Int:
                case ShaderDataType::Int2:
                case ShaderDataType::Int3:
                case ShaderDataType::Int4:
                case ShaderDataType::Bool:
                {
                    glEnableVertexArrayAttrib(m_RendererID, attribIndex);
                    glVertexArrayAttribIFormat(
                        m_RendererID,
                        attribIndex,
                        componentCount,
                        ShaderDataTypeToOpenGLBaseType(element.Type),
                        static_cast<GLuint>(element.Offset)
                    );
                    glVertexArrayAttribBinding(m_RendererID, attribIndex, bindingIndex);
                    m_VertexBufferIndex++;
                    break;
                }

                case ShaderDataType::Mat3:
                case ShaderDataType::Mat4:
                {
                    for (GLint i = 0; i < componentCount; i++)
                    {
                        const GLuint matAttribIndex = m_VertexBufferIndex;
                        glEnableVertexArrayAttrib(m_RendererID, matAttribIndex);
                        glVertexArrayAttribFormat(
                            m_RendererID,
                            matAttribIndex,
                            componentCount,
                            ShaderDataTypeToOpenGLBaseType(element.Type),
                            element.Normalized ? GL_TRUE : GL_FALSE,
                            static_cast<GLuint>(element.Offset + sizeof(float) * componentCount * i)
                        );
                        glVertexArrayAttribBinding(m_RendererID, matAttribIndex, bindingIndex);
                        glVertexAttribDivisor(matAttribIndex, 1);
                        m_VertexBufferIndex++;
                    }
                    break;
                }

                default: RS_ASSERT(false, "Unknown ShaderDataType!") break;
            }
        }

        m_VertexBuffers.push_back(vertexBuffer);
    }

    void VertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
    {
        glVertexArrayElementBuffer(m_RendererID, indexBuffer->GetRendererID());
        // Without DSA:
        // glBindVertexArray(m_RendererID);
        // indexBuffer->Bind();
        m_IndexBuffer = indexBuffer;
    }

}
