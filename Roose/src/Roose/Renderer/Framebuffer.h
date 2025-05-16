#pragma once

namespace Roose {

    enum class FramebufferTextureFormat
    {
        None = 0,

        // Color
        RGBA8,
        RGBA16F,
        RGBA32F,
        RED_INTEGER,

        // Depth/stencil
        DEPTH24STENCIL8,

        // Defaults
        Depth = DEPTH24STENCIL8
    };

    struct FramebufferTextureSpecification
    {
        FramebufferTextureSpecification() = default;
        FramebufferTextureSpecification(const FramebufferTextureFormat format)
            : TextureFormat(format) {}

        FramebufferTextureFormat TextureFormat = FramebufferTextureFormat::None;
    };

    struct FramebufferAttachmentSpecification
    {
        FramebufferAttachmentSpecification() = default;
        FramebufferAttachmentSpecification(const std::initializer_list<FramebufferTextureSpecification> attachments)
            : Attachments(attachments) {}

        std::vector<FramebufferTextureSpecification> Attachments;
    };

    struct FramebufferSpecification
    {
        uint32_t Width = 0, Height = 0;
        FramebufferAttachmentSpecification Attachments;
        uint32_t Samples = 1;

        bool SwapChainTarget = false;
    };

    class Framebuffer
    {
    public:
        Framebuffer(const FramebufferSpecification& spec);
        ~Framebuffer();

        /**
         * @brief Invalidates and recreates the framebuffer and its attachments.
         */
        void Invalidate();

        void Bind();
        void Unbind();

        /**
         * @brief Resizes the framebuffer to the specified width and height.
         * @param width The new width of the framebuffer.
         * @param height The new height of the framebuffer.
         */
        void Resize(uint32_t width, uint32_t height);

        /**
         * @brief Reads the value of a pixel from a color attachment at the specified coordinates.
         * @param attachmentIndex The index of the color attachment to read from.
         * @param x The x-coordinate of the pixel.
         * @param y The y-coordinate of the pixel.
         * @return The value of the pixel at the specified location.
         */
        int32_t ReadPixel(uint32_t attachmentIndex, int32_t x, int32_t y) const;

        /**
         * @brief Clears a color attachment to the specified integer value.
         * @param attachmentIndex The index of the color attachment to clear.
         * @param value The value to clear the attachment to.
         */
        void ClearAttachment(uint32_t attachmentIndex, int32_t value);

        /**
         * @brief Gets the renderer ID of a color attachment.
         * @param index The index of the color attachment (default is 0).
         * @return The renderer ID of the specified color attachment.
         */
        [[nodiscard]] uint32_t GetColorAttachmentRendererID(const uint32_t index = 0) const
        {
            RS_ASSERT(index < m_ColorAttachments.size())
            return m_ColorAttachments[index];
        }

        [[nodiscard]] const FramebufferSpecification& GetSpecification() const { return m_Specification; }

        static Ref<Framebuffer> Create(const FramebufferSpecification& spec);
    private:
        uint32_t m_RendererID = 0;
        FramebufferSpecification m_Specification;

        std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
        FramebufferTextureSpecification m_DepthAttachmentSpecification = FramebufferTextureFormat::None;

        std::vector<uint32_t> m_ColorAttachments;
        uint32_t m_DepthAttachment = 0;
    };

}
