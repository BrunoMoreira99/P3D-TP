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
            WavefrontOBJ obj;
            if (!obj.Load(filepath)) return;

            const std::string& mtlFilepath = obj.GetMaterialFileName();
            WavefrontMTL mtl;
            bool mtlFileLoadedSuccessfully = mtl.Load(mtlFilepath);

            for (const auto& objMesh : obj.GetMeshes())
            {
                Ref<Mesh> mesh = Mesh::LoadFromWavefrontOBJMesh(objMesh);
                // Check if a material with the same name already exists
                Ref<Material> material = MaterialLibrary::Get(objMesh.MaterialName);
                if (!material && mtlFileLoadedSuccessfully)
                {
                    // Material does not exist, create a new one
                    const WavefrontMTLMaterial* objMaterial = mtl.GetMaterial(objMesh.MaterialName);
                    if (!objMaterial)
                        RS_ERROR("[Model] Material not found: %s", objMesh.MaterialName.c_str());
                    else
                        material = Material::LoadFromWavefrontMTL(*objMaterial, objMesh.MaterialName);
                }

                m_MeshEntries.emplace_back(mesh, material, objMesh.Name);
            }
        }
        else
        {
            RS_ERROR("[Model] Unsupported file format: %s", filepath.c_str());
        }
    }

}
