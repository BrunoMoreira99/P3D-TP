#include "rspch.h"
#include "Roose/Renderer/Model.h"

#include "Roose/Importers/WavefrontMTL.h"
#include "Roose/Importers/WavefrontOBJ.h"

namespace Roose {

    Ref<Model> Model::Create(const std::string& filepath)
    {
        const std::filesystem::path path(filepath);
        if (!std::filesystem::exists(path))
        {
            RS_ERROR("[Model] File not found: %s", filepath.c_str());
            return nullptr;
        }
        if (path.extension() == ".obj")
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

        const std::string& mtlFilepath = obj.GetMaterialFileName();
        WavefrontMTL mtl;
        bool mtlFileLoadedSuccessfully = mtl.Load(mtlFilepath);

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
