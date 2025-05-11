#pragma once

#include "Roose/Importers/WavefrontOBJ.h"

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <functional>

namespace Roose {

    struct MeshVertex
    {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoord;

        bool operator==(const MeshVertex& other) const
        {
            return Position == other.Position && Normal == other.Normal && TexCoord == other.TexCoord;
        }
    };

    class Mesh
    {
    public:
        Mesh() = default;
        ~Mesh() = default;

        [[nodiscard]] const std::vector<MeshVertex>& GetVertices() const { return m_Vertices; }
        [[nodiscard]] const std::vector<uint32_t>& GetIndices() const { return m_Indices; }

        [[nodiscard]] uint32_t GetVertexCount() const { return static_cast<uint32_t>(m_Vertices.size()); }
        [[nodiscard]] uint32_t GetIndexCount() const { return static_cast<uint32_t>(m_Indices.size()); }
        [[nodiscard]] bool HasNormals() const { return m_HasNormals; }
        [[nodiscard]] bool HasTexCoords() const { return m_HasTexCoords; }

        static Ref<Mesh> Create(const WavefrontOBJMesh& objMesh);
    private:
        void LoadFromWavefrontOBJMesh(const WavefrontOBJMesh& objMesh);
    private:
        std::vector<MeshVertex> m_Vertices;
        std::vector<uint32_t> m_Indices;
        bool m_HasNormals;
        bool m_HasTexCoords;
    };

}

namespace std {
    template <>
    struct hash<Roose::MeshVertex>
    {
        size_t operator()(const Roose::MeshVertex& vertex) const noexcept
        {
            size_t seed = 0;
            std::hash<glm::vec3> vec3Hasher;
            std::hash<glm::vec2> vec2Hasher;

            // Combine the hash of each component
            seed ^= vec3Hasher(vertex.Position) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= vec3Hasher(vertex.Normal) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
            seed ^= vec2Hasher(vertex.TexCoord) + 0x9e3779b9 + (seed << 6) + (seed >> 2);

            return seed;
        }
    };
}
