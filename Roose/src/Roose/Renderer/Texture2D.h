#pragma once

#include <GL/glew.h>

namespace Roose {

    enum class ImageFormat : uint8_t
    {
        None = 0,
        R8,
        RGB8,
        RGBA8,
        RGBA32F
    };

    struct TextureSpecification
    {
        uint32_t Width = 1;
        uint32_t Height = 1;
        ImageFormat Format = ImageFormat::RGBA8;
        bool GenerateMips = true;
    };

    class Texture2D
    {
    public:
        Texture2D(const TextureSpecification& specification);
        Texture2D(const std::string& path);
        ~Texture2D();

        static Ref<Texture2D> Create(const TextureSpecification& specification);
        static Ref<Texture2D> Create(const std::string& path);

        [[nodiscard]] const TextureSpecification& GetSpecification() const { return m_Specification; }

        [[nodiscard]] uint32_t GetWidth() const { return m_Width;  }
        [[nodiscard]] uint32_t GetHeight() const { return m_Height; }
        [[nodiscard]] uint32_t GetRendererID() const { return m_RendererID; }

        [[nodiscard]] const std::string& GetPath() const { return m_Path; }

        void SetData(const void* data, uint32_t size);

        void Bind(uint32_t slot = 0) const;
        // void Unbind() const;

        [[nodiscard]] bool IsLoaded() const { return m_IsLoaded; }

        bool operator==(const Texture2D& other) const
        {
            return m_RendererID == other.GetRendererID();
        }
    private:
        TextureSpecification m_Specification;

        std::string m_Path;
        bool m_IsLoaded = false;
        uint32_t m_Width, m_Height;
        uint32_t m_RendererID;
        GLenum m_InternalFormat, m_DataFormat;
    };

}
