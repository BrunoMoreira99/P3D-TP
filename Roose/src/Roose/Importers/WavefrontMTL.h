#pragma once

#include <glm/glm.hpp>

namespace Roose {

    /**
     * @brief A simple Wavefront MTL parser.
     */
    class WavefrontMTL
    {
    public:
        WavefrontMTL() = default;
        ~WavefrontMTL() = default;

        struct Material
        {
            glm::vec3 Ka = {};   // Ambient reflectance
            glm::vec3 Kd = {};   // Diffuse reflectance
            glm::vec3 Ks = {};   // Specular reflectance
            float Ns = 1.0f;     // Shininess (specular exponent)
            std::string Texture; // Texture file
        };

        /**
         * @brief Load a Wavefront MTL file.
         * Parses the material file to extract materials by name.
         * @param filepath The path to the MTL file.
         * @return True if the file was loaded successfully, false otherwise.
         */
        bool Load(const std::string& filepath);

        /**
         * @brief Get a material by name.
         * @param name The name of the material.
         * @return A pointer to the WavefrontMTL::Material object, or nullptr if not found.
         */
        [[nodiscard]] const Material* GetMaterial(const std::string& name) const;

        /**
         * @brief Get all materials.
         * @return A map of material names to WavefrontMTL::Material objects.
         */
        [[nodiscard]] const std::unordered_map<std::string, Material>& GetAllMaterials() const { return m_Materials; }
    private:
        std::unordered_map<std::string, Material> m_Materials; // Map of material name to Material
    };

}
