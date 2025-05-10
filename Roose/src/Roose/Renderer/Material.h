#pragma once

#include "Roose/Importers/WavefrontMTL.h"
#include "Roose/Renderer/Texture2D.h"
#include "Roose/Renderer/Shader.h"
#include "Roose/Utils/ShaderUtils.h"

namespace Roose {

    class Material
    {
    public:

        struct Uniform
        {
            ShaderDataType type;
            std::string name;
            std::vector<uint8_t> value;
        };

        Material(const Ref<Shader>& shader, const std::string& name)
            : m_Shader(shader), m_Name(name) {}
        ~Material() = default;

        [[nodiscard]] const std::string& GetName() const { return m_Name; }
        [[nodiscard]] const Ref<Shader>& GetShader() const { return m_Shader; }

        template<typename T>
        void SetUniform(const std::string& name, ShaderDataType type, const T& value);
        void SetTexture(const std::string& name, const Ref<Texture2D>& texture);

        void Bind() const;

        static Ref<Material> Create(const Ref<Shader>& shader, const std::string& name);
        static Ref<Material> LoadFromWavefrontMTL(const WavefrontMTLMaterial& mtl, const std::string& name);
    private:
        Ref<Shader> m_Shader;
        std::string m_Name;
        std::vector<Uniform> m_Uniforms;
        std::unordered_map<std::string, Ref<Texture2D>> m_Textures;
    };

    class MaterialLibrary
    {
    public:
        static void Add(const std::string& name, const Ref<Material>& material);
        static void Add(const Ref<Material>& material);

        /**
         * @brief Load a material from a Wavefront MTL file.
         * @param filepath The path to the MTL file.
         * @return A reference to the loaded material, or nullptr if no material was loaded.
         * If multiple materials are found, the first one is returned.
         */
        static Ref<Material> Load(const std::string& filepath);

        [[nodiscard]] static Ref<Material> Get(const std::string& name);

        [[nodiscard]] static bool Exists(const std::string& name);
    private:
        static std::unordered_map<std::string, Ref<Material>> s_Materials;
    };

}
