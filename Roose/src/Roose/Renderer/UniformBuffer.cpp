#include "rspch.h"
#include "Roose/Renderer/UniformBuffer.h"

#include <GL/glew.h>

namespace Roose {

	Ref<UniformBuffer> UniformBuffer::Create(const uint32_t size, const uint32_t binding)
	{
		return CreateRef<UniformBuffer>(size, binding);
	}

	UniformBuffer::UniformBuffer(const uint32_t size, const uint32_t binding)
	{
		glCreateBuffers(1, &m_RendererID);
		glNamedBufferData(m_RendererID, size, nullptr, GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_RendererID);
	}

	UniformBuffer::~UniformBuffer()
	{
		glDeleteBuffers(1, &m_RendererID);
	}

	void UniformBuffer::SetData(const void* data, const uint32_t size, const uint32_t offset) const
	{
		glNamedBufferSubData(m_RendererID, offset, size, data);
	}

}
