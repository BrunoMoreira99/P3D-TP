#pragma once

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

        template<typename T>
        void SetUniform(const std::string& name, const ShaderDataType type, const T& value)
        {
            const uint32_t size = ShaderDataTypeSize(type);
            Uniform u;
            u.type = type;
            u.name = name;
            u.value.resize(size);
            memcpy(u.value.data(), &value, size);
            m_Uniforms.push_back(u);
        }
        void SetTexture(const std::string& name, const Ref<Texture2D>& texture)
        {
            m_Textures[name] = texture;
        }

        void Bind() const;

        static Ref<Material> Create(const Ref<Shader>& shader, const std::string& name);
    private:
        Ref<Shader> m_Shader;
        std::string m_Name;
        std::vector<Uniform> m_Uniforms;
        std::unordered_map<std::string, Ref<Texture2D>> m_Textures;
    };

    class MaterialLibrary
    {
    public:
        static void Add(const std::string& name, const Ref<Material>& material)
        {
            RS_ASSERT(!Exists(name), "Material with this name already exists! Material will be replaced.")
            s_Materials[name] = material;
        }
        static void Add(const Ref<Material>& material)
        {
            auto& name = material->GetName();
            Add(name, material);
        }

        static Ref<Material> Get(const std::string& name)
        {
            if (!Exists(name))
            {
                RS_ASSERT(false, "Material not found!")
                return nullptr;
            }
            return s_Materials[name];
        }

        [[nodiscard]] static bool Exists(const std::string& name)
        {
            return s_Materials.find(name) != s_Materials.end();
        }
    private:
        static std::unordered_map<std::string, Ref<Material>> s_Materials;
    };

}
