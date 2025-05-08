#pragma once

#include <glm/glm.hpp>

namespace Roose {

    class Mesh
    {
    public:
        struct Vertex
        {
            glm::vec3 Position;
            glm::vec3 Normal;
            glm::vec2 TexCoord;

            bool operator==(const Vertex& other) const
            {
                return Position == other.Position && Normal == other.Normal && TexCoord == other.TexCoord;
            }
        };

        Mesh(std::vector<Vertex>&& vertices, std::vector<uint32_t>&& indices)
            : m_Vertices(std::move(vertices)), m_Indices(std::move(indices)) {}
        ~Mesh() = default;
    private:
        std::vector<Vertex> m_Vertices;
        std::vector<uint32_t> m_Indices;
    };

}
