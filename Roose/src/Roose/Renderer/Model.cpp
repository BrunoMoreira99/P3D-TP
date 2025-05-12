#include "rspch.h"
#include "Roose/Renderer/Model.h"

#include "Roose/Importers/WavefrontMTL.h"
#include "Roose/Importers/WavefrontOBJ.h"
#include "Roose/Utils/FileSystemUtils.h"

namespace Roose {

    Ref<Model> Model::Create(const std::string& filepath)
    {
        if (!FileSystemUtils::FileExists(filepath))
        {
            RS_ERROR("[Model] File not found: %s", filepath.c_str());
            return nullptr;
        }

        if (FileSystemUtils::GetFileExtension(filepath) == ".obj")
        {
            const Ref<Model> model = CreateRef<Model>();
            model->LoadFromWavefrontOBJ(filepath);
            return model;
        }

        RS_ERROR("[Model] Unsupported file format: %s", filepath.c_str());
        return nullptr;
    }

    void Model::LoadFromWavefrontOBJ(const std::string& filepath)
    {
        WavefrontOBJ obj;
        if (!obj.Load(filepath)) return;

        WavefrontMTL mtl;
        const std::string& mtlFilePath = obj.GetMaterialFilePath();
        const bool mtlFileLoadedSuccessfully = mtlFilePath.empty() ? false : mtl.Load(mtlFilePath);

        for (const auto& objMesh : obj.GetMeshes())
        {
            Ref<Mesh> mesh = Mesh::Create(objMesh);
            if (!mesh)
            {
                RS_ERROR("[Model] Failed to create mesh from OBJ file.");
                continue;
            }
            // Check if a material with the same name already exists
            Ref<Material> material = MaterialLibrary::Get(objMesh.MaterialName);
            if (!material && mtlFileLoadedSuccessfully)
            {
                // Material does not exist, create a new one
                const WavefrontMTLMaterial* objMaterial = mtl.GetMaterial(objMesh.MaterialName);
                if (!objMaterial)
                    RS_ERROR("[Model] Material not found: %s", objMesh.MaterialName.c_str());
                else
                    material = Material::Create(*objMaterial, objMesh.MaterialName);
            }

            m_MeshEntries.emplace_back(mesh, material, objMesh.Name);
        }
    }

}
