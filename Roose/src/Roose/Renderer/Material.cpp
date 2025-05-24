#include "rspch.h"
#include "Roose/Renderer/Material.h"
#include "Roose/Renderer/Renderer.h"

namespace Roose {

    #pragma region Material
    Ref<Material> Material::Create(const std::string& name)
    {
        const Ref<Material> material = CreateRef<Material>(name);
        MaterialLibrary::Add(name, material);
        return material;
    }

    Ref<Material> Material::Create(const WavefrontMTLMaterial& mtl, const std::string& name)
    {
        const Ref<Material> material = Create(name);
        material->LoadFromWavefrontMTL(mtl);
        return material;
    }

    template<typename T>
    void Material::SetUniform(const std::string& name, const ShaderDataType type, const T& value)
    {
        const uint32_t size = ShaderDataTypeSize(type);
        Uniform u;
        u.type = type;
        u.name = name;
        u.value.resize(size);
        memcpy(u.value.data(), &value, size);
        m_Uniforms.push_back(u);
    }

    void Material::SetTexture(const std::string& name, const Ref<Texture2D>& texture)
    {
        m_Textures[name] = texture;
    }

    void Material::Bind() const
    {
        Renderer::GetMaterialDataUniformBuffer()->SetData(&m_MaterialData, sizeof(BlinnPhongMaterialData));

        if (m_Shader)
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
        }

        uint8_t textureUnit = 0;
        for (const auto& [name, texture] : m_Textures)
        {
            texture->Bind(textureUnit);
            if (m_Shader) m_Shader->SetInt(name, textureUnit);
            textureUnit++;
        }
    }

    void Material::LoadFromWavefrontMTL(const WavefrontMTLMaterial& mtl)
    {
        m_MaterialData.Diffuse = glm::vec4(mtl.Kd, 0.0f);
        m_MaterialData.Ambient = glm::vec4(mtl.Ka, 0.0f);
        m_MaterialData.Specular = glm::vec4(mtl.Ks, 0.0f);
        m_MaterialData.Shininess = mtl.Ns;

        if (!mtl.TexturePath.empty())
        {
            SetTexture("u_DiffuseTexture", Texture2D::Create(mtl.TexturePath));
        }
    }
    #pragma endregion

    #pragma region MaterialLibrary
    std::unordered_map<std::string, Ref<Material>> MaterialLibrary::s_Materials;

    void MaterialLibrary::Add(const std::string& name, const Ref<Material>& material)
    {
        RS_ASSERT(!Exists(name), "Material with this name already exists! Material will be replaced.")
        s_Materials[name] = material;
    }

    void MaterialLibrary::Add(const Ref<Material>& material)
    {
        Add(material->GetName(), material);
    }

    Ref<Material> MaterialLibrary::Load(const std::string& filepath)
    {
        WavefrontMTL mtl;
        if (!mtl.Load(filepath)) return nullptr;

        Ref<Material> firstMaterial = nullptr;
        for (const auto& [name, material] : mtl.GetAllMaterials())
        {
            if (!firstMaterial)
                firstMaterial = Material::Create(material, name);
            else
                Material::Create(material, name);
        }
        return firstMaterial;
    }

    Ref<Material> MaterialLibrary::Get(const std::string& name)
    {
        const auto it = s_Materials.find(name);
        return it != s_Materials.end() ? it->second : nullptr;
    }

    bool MaterialLibrary::Exists(const std::string& name)
    {
        return s_Materials.count(name) > 0;
    }
    #pragma endregion

}
