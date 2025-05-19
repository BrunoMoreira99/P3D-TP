#include "rspch.h"
#include "Roose/Renderer/Framebuffer.h"

#include <GL/glew.h>

namespace Roose {

    static constexpr uint32_t MaxFramebufferSize = 8192;

    namespace Utils {

        static GLenum TextureTarget(const bool multisampled)
        {
            return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
        }

        static void CreateTextures(const bool multisampled, uint32_t* outID, const uint32_t count)
        {
            glCreateTextures(TextureTarget(multisampled), static_cast<GLsizei>(count), outID);
        }

        static void AttachTexture(const uint32_t framebuffer, const uint32_t id, const uint32_t samples,
            const GLenum internalFormat, const GLenum attachmentType, const uint32_t width, const uint32_t height
        ) {
            if (samples > 1)
            {
                glTextureStorage2DMultisample(id, static_cast<GLsizei>(samples), internalFormat,
                    static_cast<GLsizei>(width), static_cast<GLsizei>(height), GL_FALSE);
            }
            else
            {
                glTextureStorage2D(id, 1, internalFormat, static_cast<GLsizei>(width), static_cast<GLsizei>(height));
                glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                glTextureParameteri(id, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
                glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            }

            glNamedFramebufferTexture(framebuffer, attachmentType, id, 0);
        }

        static bool IsDepthFormat(const FramebufferTextureFormat format)
        {
            switch (format)
            {
                case FramebufferTextureFormat::DEPTH24STENCIL8: return true;
            }

            return false;
        }

        static GLenum RooseFBTextureFormatToGL(const FramebufferTextureFormat format)
        {
            switch (format)
            {
                case FramebufferTextureFormat::RGBA8:       return GL_RGBA8;
                case FramebufferTextureFormat::RGBA16F:     return GL_RGBA16F;
                case FramebufferTextureFormat::RGBA32F:     return GL_RGBA32F;
                case FramebufferTextureFormat::RED_INTEGER: return GL_R32I;
            }

            RS_ASSERT(false)
            return 0;
        }

    }

    Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
    {
        return CreateRef<Framebuffer>(spec);
    }

    Framebuffer::Framebuffer(const FramebufferSpecification& spec) : m_Specification(spec)
    {
        for (const auto spec : m_Specification.Attachments.Attachments)
        {
            if (Utils::IsDepthFormat(spec.TextureFormat))
                m_DepthAttachmentSpecification = spec;
            else
                m_ColorAttachmentSpecifications.emplace_back(spec);
        }

        Invalidate();
    }

    Framebuffer::~Framebuffer()
    {
        glDeleteFramebuffers(1, &m_RendererID);
        glDeleteTextures(static_cast<GLsizei>(m_ColorAttachments.size()), m_ColorAttachments.data());
        glDeleteTextures(1, &m_DepthAttachment);
    }

    void Framebuffer::Invalidate()
    {
        if (m_RendererID)
        {
            glDeleteFramebuffers(1, &m_RendererID);
            glDeleteTextures(static_cast<GLsizei>(m_ColorAttachments.size()), m_ColorAttachments.data());
            glDeleteTextures(1, &m_DepthAttachment);

            m_ColorAttachments.clear();
            m_DepthAttachment = 0;
        }

        glCreateFramebuffers(1, &m_RendererID);

        const bool multisample = m_Specification.Samples > 1;

        // Attachments
        if (!m_ColorAttachmentSpecifications.empty())
        {
            m_ColorAttachments.resize(m_ColorAttachmentSpecifications.size());
            Utils::CreateTextures(multisample, m_ColorAttachments.data(), static_cast<uint32_t>(m_ColorAttachments.size()));

            for (uint32_t i = 0; i < m_ColorAttachments.size(); i++)
            {
                Utils::AttachTexture(m_RendererID, m_ColorAttachments[i], m_Specification.Samples,
                    Utils::RooseFBTextureFormatToGL(m_ColorAttachmentSpecifications[i].TextureFormat),
                    GL_COLOR_ATTACHMENT0 + i, m_Specification.Width, m_Specification.Height);
            }
        }

        if (m_DepthAttachmentSpecification.TextureFormat != FramebufferTextureFormat::None)
        {
            Utils::CreateTextures(multisample, &m_DepthAttachment, 1);
            switch (m_DepthAttachmentSpecification.TextureFormat)
            {
                case FramebufferTextureFormat::DEPTH24STENCIL8:
                    Utils::AttachTexture(m_RendererID, m_DepthAttachment, m_Specification.Samples,
                        GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL_ATTACHMENT,
                        m_Specification.Width, m_Specification.Height);
                    break;
            }
        }

        if (m_ColorAttachments.size() > 1)
        {
            RS_ASSERT(m_ColorAttachments.size() <= 4)
            constexpr GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
            glNamedFramebufferDrawBuffers(m_RendererID, static_cast<GLsizei>(m_ColorAttachments.size()), buffers);
        }
        else if (m_ColorAttachments.empty())
        {
            // Only depth-pass
            glNamedFramebufferDrawBuffer(m_RendererID, GL_NONE);
        }

        RS_ASSERT(glCheckNamedFramebufferStatus(m_RendererID, GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!")
    }

    void Framebuffer::Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
        glViewport(0, 0, static_cast<GLsizei>(m_Specification.Width), static_cast<GLsizei>(m_Specification.Height));
    }

    void Framebuffer::Unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void Framebuffer::Resize(const uint32_t width, const uint32_t height)
    {
        if (width == 0 || height == 0 || width > MaxFramebufferSize || height > MaxFramebufferSize)
        {
            RS_INFO("Attempted to resize framebuffer to %u, %u", width, height);
            return;
        }
        m_Specification.Width = width;
        m_Specification.Height = height;

        Invalidate();
    }

    int32_t Framebuffer::ReadPixel(const uint32_t attachmentIndex, const int32_t x, const int32_t y) const
    {
        RS_ASSERT(attachmentIndex < m_ColorAttachments.size())

        glNamedFramebufferReadBuffer(m_RendererID, GL_COLOR_ATTACHMENT0 + attachmentIndex);
        int32_t pixelData;
        glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
        return pixelData;
    }

    void Framebuffer::ClearAttachment(const uint32_t attachmentIndex, const int32_t value)
    {
        RS_ASSERT(attachmentIndex < m_ColorAttachments.size())

        const auto& spec = m_ColorAttachmentSpecifications[attachmentIndex];
        glClearTexImage(m_ColorAttachments[attachmentIndex], 0,
            Utils::RooseFBTextureFormatToGL(spec.TextureFormat), GL_INT, &value);
    }

}
