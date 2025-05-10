#include "rspch.h"
#include "Roose/Renderer/Mesh.h"

namespace Roose {

    Ref<Mesh> Mesh::Create(const WavefrontOBJMesh& objMesh)
    {
        if (objMesh.Faces.empty())
        {
            RS_ERROR("[Mesh] No faces found in OBJ file.");
            return nullptr;
        }
        Ref<Mesh> mesh = CreateRef<Mesh>();
        mesh->LoadFromWavefrontOBJMesh(objMesh);
        return mesh;
    }

    void Mesh::LoadFromWavefrontOBJMesh(const WavefrontOBJMesh& objMesh)
    {
        const WavefrontOBJ& parentOBJ = objMesh.GetParent();
        const auto& objVerticesData  = parentOBJ.GetVertices();
        const auto& objNormalsData   = parentOBJ.GetNormals();
        const auto& objTexCoordsData = parentOBJ.GetTexCoords();

        if (objVerticesData.empty())
        {
            RS_ERROR("[Mesh] No vertices found in OBJ file.");
            return;
        }

        m_HasNormals = !objNormalsData.empty();
        m_HasTexCoords = !objTexCoordsData.empty();

        std::unordered_map<MeshVertex, uint32_t> uniqueVertices;

        for (const auto& face : objMesh.Faces)
        {
            for (const auto& vertexIndex : face)
            {
                MeshVertex meshVertex;
                meshVertex.Position = objVerticesData[vertexIndex.Position];
                if (m_HasNormals)
                    meshVertex.Normal = objNormalsData[vertexIndex.Normal];
                if (m_HasTexCoords)
                    meshVertex.TexCoord = objTexCoordsData[vertexIndex.TexCoord];

                // Vertex deduplication
                if (uniqueVertices.count(meshVertex) == 0)
                {
                    uniqueVertices[meshVertex] = static_cast<uint32_t>(m_Vertices.size());
                    m_Vertices.push_back(meshVertex);
                }
                m_Indices.push_back(uniqueVertices[meshVertex]);
            }
        }
    }

}
