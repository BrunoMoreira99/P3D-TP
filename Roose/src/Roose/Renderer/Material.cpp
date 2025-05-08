#include "rspch.h"
#include "Roose/Renderer/Material.h"

namespace Roose {

    std::unordered_map<std::string, Ref<Material>> MaterialLibrary::s_Materials;

    Ref<Material> Material::Create(const Ref<Shader>& shader, const std::string& name)
    {
        const Ref<Material> material = CreateRef<Material>(shader, name);
        MaterialLibrary::Add(name, material);
        return material;
    }

    void Material::Bind() const
    {
        m_Shader->Bind();
        for (const auto& [type, name, value] : m_Uniforms)
        {
            const void* data = value.data();
            switch (type)
            {
                case ShaderDataType::Float:  m_Shader->SetFloat (name, *static_cast<const float*>(data));      break;
                case ShaderDataType::Float2: m_Shader->SetFloat2(name, *static_cast<const glm::vec2*>(data));  break;
                case ShaderDataType::Float3: m_Shader->SetFloat3(name, *static_cast<const glm::vec3*>(data));  break;
                case ShaderDataType::Float4: m_Shader->SetFloat4(name, *static_cast<const glm::vec4*>(data));  break;
                case ShaderDataType::Mat3:   m_Shader->SetMat3  (name, *static_cast<const glm::mat3*>(data));  break;
                case ShaderDataType::Mat4:   m_Shader->SetMat4  (name, *static_cast<const glm::mat4*>(data));  break;
                case ShaderDataType::Int:    m_Shader->SetInt   (name, *static_cast<const int*>(data));        break;
                case ShaderDataType::Int2:   m_Shader->SetInt2  (name, *static_cast<const glm::ivec2*>(data)); break;
                case ShaderDataType::Int3:   m_Shader->SetInt3  (name, *static_cast<const glm::ivec3*>(data)); break;
                case ShaderDataType::Int4:   m_Shader->SetInt4  (name, *static_cast<const glm::ivec4*>(data)); break;
                case ShaderDataType::Bool:   m_Shader->SetBool  (name, *static_cast<const bool*>(data));       break;
                default: RS_ASSERT(false, "Unknown ShaderDataType!")                                           break;
            }
        }
        uint8_t textureUnit = 0;
        for (const auto& [name, texture] : m_Textures)
        {
            texture->Bind(textureUnit);
            m_Shader->SetInt(name, textureUnit);
            textureUnit++;
        }
    }

}
