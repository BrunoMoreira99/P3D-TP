#pragma once

#include <glm/glm.hpp>

namespace Roose {

    struct WavefrontMTLMaterial
    {
        glm::vec3 Ka = {};   // Ambient reflectance
        glm::vec3 Kd = {};   // Diffuse reflectance
        glm::vec3 Ks = {};   // Specular reflectance
        float Ns = 1.0f;     // Shininess (specular exponent)
        std::string Texture; // Texture file
    };

    /**
     * @brief A simple Wavefront MTL parser.
     */
    class WavefrontMTL
    {
    public:
        WavefrontMTL() = default;
        ~WavefrontMTL() = default;

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
         * @return A pointer to the WavefrontMTLMaterial object, or nullptr if not found.
         */
        [[nodiscard]] const WavefrontMTLMaterial* GetMaterial(const std::string& name) const;

        /**
         * @brief Get all materials.
         * @return A map of material names to WavefrontMTLMaterial objects.
         */
        [[nodiscard]] const std::unordered_map<std::string, WavefrontMTLMaterial>& GetAllMaterials() const { return m_Materials; }
    private:
        std::unordered_map<std::string, WavefrontMTLMaterial> m_Materials;
    };

}
