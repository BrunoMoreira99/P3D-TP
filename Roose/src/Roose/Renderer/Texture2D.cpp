#include "rspch.h"
#include "Roose/Renderer/Texture2D.h"

#include <stb_image.h>

namespace Roose {

    namespace Utils {

        static GLenum RooseImageFormatToGLDataFormat(const ImageFormat format)
        {
            switch (format)
            {
                case ImageFormat::RGB8:  return GL_RGB;
                case ImageFormat::RGBA8: return GL_RGBA;
            }

            RS_ASSERT(false)
            return 0;
        }

        static GLenum RooseImageFormatToGLInternalFormat(const ImageFormat format)
        {
            switch (format)
            {
                case ImageFormat::RGB8:  return GL_RGB8;
                case ImageFormat::RGBA8: return GL_RGBA8;
            }

            RS_ASSERT(false)
            return 0;
        }

        static int32_t CalculateMipMapCount(const uint32_t width, const uint32_t height)
        {
            return 1 + static_cast<int32_t>(std::floor(std::log2(std::max(width, height))));
        }

    }

    Ref<Texture2D> Texture2D::Create(const TextureSpecification& specification)
    {
        return CreateRef<Texture2D>(specification);
    }

    Ref<Texture2D> Texture2D::Create(const std::string& path)
    {
        return CreateRef<Texture2D>(path);
    }

    Texture2D::Texture2D(const TextureSpecification& specification)
        : m_Specification(specification), m_Width(m_Specification.Width), m_Height(m_Specification.Height)
    {
        m_InternalFormat = Utils::RooseImageFormatToGLInternalFormat(m_Specification.Format);
        m_DataFormat = Utils::RooseImageFormatToGLDataFormat(m_Specification.Format);

        const int32_t mipLevels = m_Specification.GenerateMips ? Utils::CalculateMipMapCount(m_Width, m_Height) : 1;
        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
        glTextureStorage2D(m_RendererID, mipLevels, m_InternalFormat,
            static_cast<GLsizei>(m_Width), static_cast<GLsizei>(m_Height));

        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, m_Specification.GenerateMips ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    Texture2D::Texture2D(const std::string& path) : m_Path(path)
    {
        int width, height, channels;
        stbi_set_flip_vertically_on_load(1);
        stbi_uc* data = nullptr;
        {
            data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        }

        if (data)
        {
            m_IsLoaded = true;

            m_Width = width;
            m_Height = height;

            GLenum internalFormat = 0, dataFormat = 0;
            ImageFormat specFormat = ImageFormat::None;
            if (channels == 4)
            {
                internalFormat = GL_RGBA8;
                dataFormat = GL_RGBA;
                specFormat = ImageFormat::RGBA8;
            }
            else if (channels == 3)
            {
                internalFormat = GL_RGB8;
                dataFormat = GL_RGB;
                specFormat = ImageFormat::RGB8;
            }

            m_InternalFormat = internalFormat;
            m_DataFormat = dataFormat;

            RS_ASSERT(internalFormat & dataFormat, "Format not supported!")

            // Populate m_Specification
            m_Specification.Width = m_Width;
            m_Specification.Height = m_Height;
            m_Specification.Format = specFormat;
            m_Specification.GenerateMips = true;

            int mipLevels = Utils::CalculateMipMapCount(m_Width, m_Height);
            glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
            glTextureStorage2D(m_RendererID, mipLevels, internalFormat,
                static_cast<GLsizei>(m_Width), static_cast<GLsizei>(m_Height));

            glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

            glTextureSubImage2D(m_RendererID, 0, 0, 0,
                static_cast<GLsizei>(m_Width), static_cast<GLsizei>(m_Height), dataFormat, GL_UNSIGNED_BYTE, data);

            glGenerateTextureMipmap(m_RendererID);

            stbi_image_free(data);
        }
    }

    Texture2D::~Texture2D()
    {
        glDeleteTextures(1, &m_RendererID);
    }

    void Texture2D::SetData(const void* data, const uint32_t size)
    {
        const uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
        RS_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!")
        glTextureSubImage2D(m_RendererID, 0, 0, 0,
            static_cast<GLsizei>(m_Width), static_cast<GLsizei>(m_Height), m_DataFormat, GL_UNSIGNED_BYTE, data);
        if (m_Specification.GenerateMips) {
            glGenerateTextureMipmap(m_RendererID);
        }
    }

    void Texture2D::Bind(const uint32_t slot) const
    {
        glBindTextureUnit(slot, m_RendererID);
        // glBindTextureUnit was introduced in OpenGL 4.5 and does the same as:
        // glActiveTexture(GL_TEXTURE0 + slot);
        // glBindTexture(GL_TEXTURE_2D, m_RendererID);
    }

}
