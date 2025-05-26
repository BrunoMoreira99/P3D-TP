#pragma once

#include "Roose/Renderer/Material.h"
#include "Roose/Renderer/Mesh.h"

namespace Roose {

    class Model
    {
    public:
        struct MeshEntry {
            Ref<Mesh> Mesh;
            Ref<Material> Material;
            std::string Name;

            MeshEntry(const Ref<Roose::Mesh>& mesh, const Ref<Roose::Material>& material, std::string name)
                : Mesh(mesh), Material(material), Name(std::move(name)) {}
        };

        Model() = default;

        /**
         * @brief Get the list of meshes in this model. This is a list of MeshEntry objects sorted by material.
         */
        [[nodiscard]] std::vector<MeshEntry>& GetMeshes() { return m_MeshEntries; }

        static Ref<Model> Create(const std::string& filepath);
    private:
        void LoadFromWavefrontOBJ(const std::string& filepath);
    private:
        std::vector<MeshEntry> m_MeshEntries;
    };

}
