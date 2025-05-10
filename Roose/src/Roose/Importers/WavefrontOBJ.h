#pragma once

#include <glm/glm.hpp>

namespace Roose {

    class WavefrontOBJ;

    struct WavefrontOBJFaceVertexIndex
    {
        uint32_t Position;
        uint32_t TexCoord;
        uint32_t Normal;
    };

    struct WavefrontOBJMesh
    {
        std::string Name;
        std::vector<std::vector<WavefrontOBJFaceVertexIndex>> Faces;
        std::string MaterialName;

        WavefrontOBJMesh(WavefrontOBJ* parent)
            : m_Parent(parent) {}

        [[nodiscard]] const WavefrontOBJ& GetParent() const { return *m_Parent; }
    private:
        WavefrontOBJ* m_Parent = nullptr;
    };

    /**
     * @brief A simple Wavefront OBJ parser.
     */
    class WavefrontOBJ
    {
    public:
        WavefrontOBJ() = default;
        ~WavefrontOBJ() = default;

        /**
         * @brief Load a Wavefront OBJ file.
         * @param filepath The path to the OBJ file.
         * @return True if the file was loaded successfully, false otherwise.
         */
        bool Load(const std::string& filepath);

        /**
         * @brief Get the global vertices.
         * @return A vector of vertex positions.
         */
        [[nodiscard]] const std::vector<glm::vec3>& GetVertices() const { return m_Vertices; }

        /**
         * @brief Get the global normals.
         * Normals are optional in OBJ files, so this may be empty if not provided.
         * @return A vector of normal vectors.
         */
        [[nodiscard]] const std::vector<glm::vec3>& GetNormals() const { return m_Normals; }

        /**
         * @brief Get the global texture coordinates.
         * Texture coordinates are optional in OBJ files, so this may be empty if not provided.
         * @return A vector of texture coordinate vectors.
         */
        [[nodiscard]] const std::vector<glm::vec2>& GetTexCoords() const { return m_TexCoords; }

        /**
         * @brief Get the list of meshes.
         * Each mesh contains its own faces and associated material name.
         * @return A vector of WavefrontOBJMesh objects.
         */
        [[nodiscard]] const std::vector<WavefrontOBJMesh>& GetMeshes() const { return m_Meshes; }

        /**
         * @brief Get the material file name.
         * @return A string containing the material file name, or an empty string if no material file is specified.
         */
        [[nodiscard]] const std::string& GetMaterialFileName() const { return m_MaterialFileName; }
    private:
        std::vector<glm::vec3> m_Vertices; // Global vertex positions
        std::vector<glm::vec3> m_Normals;  // Global normals
        std::vector<glm::vec2> m_TexCoords; // Global texture coordinates
        std::vector<WavefrontOBJMesh> m_Meshes; // List of meshes
        std::string m_MaterialFileName; // Name of the material file (.mtl)
    };

}
