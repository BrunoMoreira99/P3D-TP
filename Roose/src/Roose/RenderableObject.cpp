#include "rspch.h"
#include "RenderableObject.h"

#include "Roose/Renderer/Model.h"
#include "glm/gtx/quaternion.hpp"

namespace Roose {

    void RenderableObject::Load(std::string obj_model_filepath)
    {
        const Ref<Model> model = Model::Create(obj_model_filepath);
        if (!model) return;
        m_MeshesEntries = std::vector(model->GetMeshes());
    }

    void RenderableObject::Install()
    {
        uint32_t vertexCount = 0;
        uint32_t indexCount = 0;

        // Calculate total vertex and index counts
        for (const auto& meshEntry : m_MeshesEntries)
        {
            vertexCount += meshEntry.Mesh->GetVertexCount();
            indexCount += meshEntry.Mesh->GetIndexCount();
        }

        // Create buffers
        m_VBO = VertexBuffer::Create(vertexCount * sizeof(MeshVertex));
        m_VBO->SetLayout({
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float3, "a_Normal" },
            { ShaderDataType::Float2, "a_TexCoord" }
        });
        m_IBO = IndexBuffer::Create(indexCount);

        uint32_t vertexOffset = 0;
        uint32_t indexOffset = 0;

        // Populate vertices and indices
        for (const auto& meshEntry : m_MeshesEntries)
        {
            const auto& meshVertices = meshEntry.Mesh->GetVertices();
            auto meshIndices = meshEntry.Mesh->GetIndices(); // Copy to modify indices
            const uint32_t meshVertexCount = static_cast<uint32_t>(meshVertices.size());
            const uint32_t meshIndexCount = static_cast<uint32_t>(meshIndices.size());

            // Shift indices by the current vertex offset
            for (auto& index : meshIndices)
                index += vertexOffset;

            // Upload vertex and index data to GPU
            m_VBO->SetData(
                meshVertices.data(),
                meshVertexCount * sizeof(MeshVertex),
                vertexOffset * sizeof(MeshVertex)
            );
            // Note that while the VertexBuffer requires the size and offset in bytes explicitly, the IndexBuffer does not.
            // Since the data format of a VertexBuffer can wildly differ, we need to specify the size and offset in bytes
            // explicitly every time. The data of an IndexBuffer, however, are always 32-bit unsigned integers (in our case).
            // This lets the IndexBuffer class calculate the size and offset in bytes internally.
            m_IBO->SetData(meshIndices.data(), meshIndexCount, indexOffset);

            vertexOffset += meshVertexCount;
            indexOffset += meshIndexCount;
        }

        // Create the Vertex Array Object (VAO) and bind the VBO and IBO to it
        m_VAO = VertexArray::Create();
        m_VAO->AddVertexBuffer(m_VBO);
        m_VAO->SetIndexBuffer(m_IBO);
    }

    void RenderableObject::Render(const glm::vec3 position, const glm::vec3 orientation) const
    {
        // Make transform matrix out of position and orientation
        Render(glm::translate(glm::mat4(1.0f), position) * glm::toMat4(glm::quat(orientation)));
    }

    void RenderableObject::Render(const glm::mat4& transform) const
    {
        if (m_MeshesEntries.empty()) return;
        m_VAO->Bind();

        uint32_t indexOffset = 0;
        size_t i = 0;
        while (i < m_MeshesEntries.size()) {
            const auto& currentMaterial = m_MeshesEntries[i].Material;
            uint32_t groupIndexCount = 0;
            size_t j = i;
            // Accumulate index count for contiguous meshes with the same material (m_MeshesEntries is sorted by material)
            while (j < m_MeshesEntries.size() && m_MeshesEntries[j].Material == currentMaterial) {
                groupIndexCount += m_MeshesEntries[j].Mesh->GetIndexCount();
                ++j;
            }
            // Bind once for this material and draw all meshes with the same material at once
            currentMaterial->Bind();
            currentMaterial->GetShader()->SetMat4("u_Transform", transform);
            glDrawElements(
                GL_TRIANGLES,
                static_cast<GLsizei>(groupIndexCount),
                GL_UNSIGNED_INT,
                reinterpret_cast<GLvoid*>(indexOffset * sizeof(uint32_t))
            );
            indexOffset += groupIndexCount;
            i = j;
        }
    }

    void RenderableObject::SetMaterial(const Ref<Material>& material)
    {
        for (auto& meshEntry : m_MeshesEntries)
            meshEntry.Material = material;
    }

    void RenderableObject::SetMaterial(const Ref<Material>& material, const std::string& meshName)
    {
        for (auto& meshEntry : m_MeshesEntries)
        {
            if (meshEntry.Name == meshName)
            {
                meshEntry.Material = material;
                return;
            }
        }
        RS_ERROR("Mesh with name %s not found in RenderableObject", meshName.c_str());
    }

}
