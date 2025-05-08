#pragma once

#include "Roose/Renderer/Material.h"
#include "Roose/Renderer/Mesh.h"

namespace Roose {

    class Model
    {
    public:
        struct MeshEntry {
            Ref<Mesh> mesh;
            Ref<Material> material;
            std::string name;

            MeshEntry(const Ref<Mesh>& mesh, const Ref<Material>& material, std::string name)
                : mesh(mesh), material(material), name(std::move(name)) {}
        };

        Model(const std::string& filepath);
        ~Model() = default;

        [[nodiscard]] const std::vector<MeshEntry>& GetMeshes() const { return m_MeshEntries; }
    private:
        void LoadFromWavefrontOBJ(const std::string& filepath);
    private:
        std::vector<MeshEntry> m_MeshEntries;
    };

}
