#include "rspch.h"
#include "Roose/Renderer/Model.h"

#include "Roose/Importers/WavefrontMTL.h"
#include "Roose/Importers/WavefrontOBJ.h"

namespace Roose {

    Model::Model(const std::string& filepath)
    {
        const std::filesystem::path path(filepath);
        if (!std::filesystem::exists(path))
        {
            RS_ERROR("[Model] File not found: %s", filepath.c_str());
            return;
        }
        if (path.extension() == ".obj")
        {
            LoadFromWavefrontOBJ(filepath);
        }
        else
        {
            RS_ERROR("[Model] Unsupported file format: %s", filepath.c_str());
        }
    }

    void Model::LoadFromWavefrontOBJ(const std::string& filepath)
    {
        WavefrontOBJ obj;
        if (!obj.Load(filepath)) return;

        const std::vector<glm::vec3>& objVerticesData = obj.GetVertices();
        const std::vector<glm::vec3>& objNormalsData = obj.GetNormals();
        const std::vector<glm::vec2>& objTexCoordsData = obj.GetTexCoords();
        const std::string& mtlFilepath = obj.GetMaterialFileName();

        WavefrontMTL mtl;
        bool mtlFileLoadedSuccessfully = mtl.Load(mtlFilepath);

        for (const auto& objMesh : obj.GetMeshes())
        {
            std::vector<Mesh::Vertex> vertices;
            std::vector<uint32_t> indices;

            for (const auto& face : objMesh.faces)
            {
                for (const auto& vertexIndex : face)
                {
                    Mesh::Vertex v;
                    v.Position = objVerticesData[vertexIndex.position];
                    if (!objNormalsData.empty())
                        v.Normal = objNormalsData[vertexIndex.normal];
                    if (!objTexCoordsData.empty())
                        v.TexCoord = objTexCoordsData[vertexIndex.texCoord];

                    vertices.push_back(v);
                    indices.push_back(static_cast<uint32_t>(vertices.size() - 1));
                }
            }

            Ref<Material> material;
            if (mtlFileLoadedSuccessfully)
            {
                const WavefrontMTL::Material* objMaterial = mtl.GetMaterial(objMesh.materialName);
                if (!objMaterial)
                {
                    RS_ERROR("[Model::LoadFromWavefrontOBJ] Material not found: %s", objMesh.materialName.c_str());
                }
                else
                {
                    material = Material::Create(nullptr, objMesh.materialName);
                    material->SetUniform("u_AmbientReflectance", ShaderDataType::Float3, objMaterial->Ka);
                    material->SetUniform("u_DiffuseReflectance", ShaderDataType::Float3, objMaterial->Kd);
                    material->SetUniform("u_SpecularReflectance", ShaderDataType::Float3, objMaterial->Ks);
                    material->SetUniform("u_SpecularExponent", ShaderDataType::Float, objMaterial->Ns);

                    if (!objMaterial->Texture.empty())
                    {
                        Ref<Texture2D> texture = Texture2D::Create(objMaterial->Texture);
                        material->SetTexture("u_Texture", texture);
                    }
                }
            }

            Ref<Mesh> mesh = CreateRef<Mesh>(std::move(vertices), std::move(indices));
            m_MeshEntries.emplace_back(mesh, material, objMesh.name);
        }
    }

}
