#pragma once

namespace Roose {

    class IndexBuffer
    {
    public:
        IndexBuffer(const uint32_t* indices, uint32_t count);
        ~IndexBuffer();

        void Bind() const;
        void Unbind() const;

        [[nodiscard]] uint32_t GetRendererID() const { return m_RendererID; }
        [[nodiscard]] uint32_t GetCount() const { return m_Count; }
    private:
        uint32_t m_RendererID;
        uint32_t m_Count;
    };

}
