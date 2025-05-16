#pragma once

#include "Roose/Importers/WavefrontMTL.h"
#include "Roose/Renderer/Texture2D.h"
#include "Roose/Renderer/Shader.h"
#include "Roose/Utils/ShaderUtils.h"

namespace Roose {

    class Material
    {
    public:

        struct BlinnPhongMaterialData
        {
            glm::vec4 Diffuse;
            glm::vec4 Ambient;
            glm::vec4 Specular;
            float Shininess;
        };

        struct Uniform
        {
            ShaderDataType type;
            std::string name;
            std::vector<uint8_t> value;
        };

        Material(const std::string& name)
            : m_Name(name) {}

        [[nodiscard]] const std::string& GetName() const { return m_Name; }
        [[nodiscard]] const Ref<Shader>& GetShader() const { return m_Shader; }

        template<typename T>
        void SetUniform(const std::string& name, ShaderDataType type, const T& value);
        void SetTexture(const std::string& name, const Ref<Texture2D>& texture);
        void SetShader(const Ref<Shader>& shader) { m_Shader = shader; }

        void Bind() const;

        static Ref<Material> Create(const std::string& name);
        static Ref<Material> Create(const WavefrontMTLMaterial& mtl, const std::string& name);
    private:
        void LoadFromWavefrontMTL(const WavefrontMTLMaterial& mtl);
    private:
        std::string m_Name;
        std::vector<Uniform> m_Uniforms;
        std::unordered_map<std::string, Ref<Texture2D>> m_Textures;
        BlinnPhongMaterialData m_MaterialData = {};
        Ref<Shader> m_Shader = nullptr;
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
