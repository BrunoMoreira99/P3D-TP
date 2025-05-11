#pragma once

#include "Roose/Renderer/Model.h"
#include "Roose/Renderer/VertexArray.h"
#include "Roose/Renderer/VertexBuffer.h"
#include <glm/glm.hpp>

namespace Roose {

    class RenderableObject
    {
    public:
        RenderableObject() = default;
        ~RenderableObject() = default;

        /**
         * @brief Load a 3D model from a file, as well as its associated materials.
         * @param obj_model_filepath The path to the OBJ model file.
         */
        void Load(std::string obj_model_filepath);

        /**
         * @brief Generates the necessary OpenGL objects / buffers and uploads the mesh data to the GPU.
         */
        void Install();

        /**
         * @brief Renders the object at the specified position and orientation.
         * @param position The position to render the object at.
         * @param orientation The orientation of the object.
         */
        void Render(glm::vec3 position, glm::vec3 orientation) const;

        /**
         * @brief Renders the object with the specified transformation matrix.
         * @param transform The transformation matrix to apply to the object.
         */
        void Render(const glm::mat4& transform) const;

        /**
         * @brief Sets the material for all meshes in this object.
         * @param material The material to set for this object.
         */
        void SetMaterial(const Ref<Material>& material);

        /**
         * @brief Sets the material for a specific mesh in this object.
         * @param material The material to set for this object.
         * @param meshName The name of the mesh to set the material for.
         */
        void SetMaterial(const Ref<Material>& material, const std::string& meshName);
    private:
        std::vector<Model::MeshEntry> m_MeshesEntries;
        Ref<VertexArray> m_VAO;
        Ref<VertexBuffer> m_VBO;
        Ref<IndexBuffer> m_IBO;
    };

}
