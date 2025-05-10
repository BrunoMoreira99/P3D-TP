#pragma once

#include <glm/glm.hpp>
#include "Roose/Renderer/Material.h"
#include "Roose/Renderer/Mesh.h"
#include "Roose/Renderer/VertexArray.h"
#include "Roose/Renderer/VertexBuffer.h"

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
        void Render(glm::vec3 position, glm::vec3 orientation);

        /**
         * @brief Renders the object with the specified transformation matrix.
         * @param transform The transformation matrix to apply to the object.
         */
        void Render(glm::mat4 transform);

        /**
         * @brief Sets the mesh for this object.
         * @param material The material to set for this object.
         */
        void SetMaterial(Ref<Material> material);
    private:
        Ref<Mesh> m_Mesh;
        Ref<Material> m_Material;
        uint32_t m_Count = 0;
        uint32_t m_Offset = 0;

        static Ref<VertexArray> s_VAO;
        static Ref<VertexBuffer> s_VBO;
        static Ref<IndexBuffer> s_IBO;
    };

}
