#pragma once

namespace Roose {

    class IndexBuffer
    {
    public:
        IndexBuffer(uint32_t count);
        IndexBuffer(const uint32_t* indices, uint32_t count);
        ~IndexBuffer();

        void Bind() const;
        void Unbind() const;

        [[nodiscard]] uint32_t GetRendererID() const { return m_RendererID; }

        /**
         * @brief Returns the number of indices in the buffer.
         * @return The number of indices.
         */
        [[nodiscard]] uint32_t GetCount() const { return m_Count; }

        /**
         * @brief Sets the data for the index buffer.
         * @param indices The pointer to the list of indices.
         * @param count The number of indices to set.
         * @param offset The offset in the buffer to start writing data. This offset is not in bytes, but in number of indices.
         */
        void SetData(const uint32_t* indices, uint32_t count, uint32_t offset = 0);

        /**
         * @brief Creates an IndexBuffer with enough size to accommodate the specified number of indices.
         * @param count The number of indices.
         * @return A reference to the created IndexBuffer.
         */
        static Ref<IndexBuffer> Create(uint32_t count);

        /**
         * @brief Creates an IndexBuffer from a list of indices.
         * @param indices The pointer to the list of indices.
         * @param count The number of indices.
         * @return A reference to the created IndexBuffer.
         */
        static Ref<IndexBuffer> Create(const uint32_t* indices, uint32_t count);
    private:
        uint32_t m_RendererID;
        uint32_t m_Count;
    };

}
