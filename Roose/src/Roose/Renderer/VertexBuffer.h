#pragma once

#include "Roose/Utils/ShaderUtils.h"

#include <GL/glew.h>

namespace Roose {

	struct BufferElement
	{
		std::string Name;
		ShaderDataType Type;
		uint32_t Size;
		size_t Offset;
		bool Normalized;

		BufferElement() = default;

		BufferElement(const ShaderDataType type, const std::string& name, const bool normalized = false)
			: Name(name), Type(type), Size(ShaderDataTypeSize(type)), Offset(0), Normalized(normalized) {}

		[[nodiscard]] uint32_t GetComponentCount() const
		{
			switch (Type)
			{
				case ShaderDataType::Float:  return 1;
				case ShaderDataType::Float2: return 2;
				case ShaderDataType::Float3: return 3;
				case ShaderDataType::Float4: return 4;
				case ShaderDataType::Mat3:   return 3; // 3 * float3
				case ShaderDataType::Mat4:   return 4; // 4 * float4
				case ShaderDataType::Int:    return 1;
				case ShaderDataType::Int2:   return 2;
				case ShaderDataType::Int3:   return 3;
				case ShaderDataType::Int4:   return 4;
				case ShaderDataType::Bool:   return 1;
			}

			RS_ASSERT(false, "Unknown ShaderDataType!")
			return 0;
		}
	};

	class BufferLayout
	{
	public:
		BufferLayout() = default;

		BufferLayout(const std::initializer_list<BufferElement> elements)
			: m_Elements(elements)
		{
			CalculateOffsetsAndStride();
		}

		[[nodiscard]] uint32_t GetStride() const { return m_Stride; }
		[[nodiscard]] const std::vector<BufferElement>& GetElements() const { return m_Elements; }

		std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }
	private:
		void CalculateOffsetsAndStride()
		{
			size_t offset = 0;
			m_Stride = 0;
			for (auto& element : m_Elements)
			{
				element.Offset = offset;
				offset += element.Size;
				m_Stride += element.Size;
			}
		}
	private:
		std::vector<BufferElement> m_Elements;
		uint32_t m_Stride = 0;
	};

    class VertexBuffer
    {
    public:
    	VertexBuffer(uint32_t size);
        VertexBuffer(const void* vertices, uint32_t size);
        ~VertexBuffer();

        void Bind() const;
        void Unbind() const;

	    /**
	     * @brief Sets the data for the vertex buffer.
	     * @param data The pointer to the data to set.
	     * @param size The size in bytes of the data to set.
	     * @param offset The offset in the buffer to start writing data. This offset is in bytes.
	     */
	    void SetData(const void* data, uint32_t size, uint32_t offset = 0);

    	[[nodiscard]] uint32_t GetRendererID() const { return m_RendererID; }
    	[[nodiscard]] const BufferLayout& GetLayout() const { return m_Layout; }
    	void SetLayout(const BufferLayout& layout) { m_Layout = layout; }

    	/**
		 * @brief Creates a VertexBuffer with the specified size.
		 * @param size The size in bytes of the buffer object's data store.
		 * @return A reference to the created VertexBuffer.
		 */
		[[nodiscard]] static Ref<VertexBuffer> Create(uint32_t size);

	    /**
	     * @brief Creates a VertexBuffer with the specified vertices and size.
	     * @param vertices The pointer to data that will be copied into the data store for initialization,
	     * or nullptr if no data is to be copied.
	     * @param size The size in bytes of the buffer object's data store.
	     * @return A reference to the created VertexBuffer.
	     */
	    [[nodiscard]]static Ref<VertexBuffer> Create(const void* vertices, uint32_t size);
    private:
        uint32_t m_RendererID;
    	uint32_t m_Size;
    	BufferLayout m_Layout;
    };

}
