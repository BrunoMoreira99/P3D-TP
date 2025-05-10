#include "rspch.h"
#include "Roose/Renderer/Mesh.h"

namespace Roose {

    Ref<Mesh> Mesh::LoadFromWavefrontOBJMesh(const WavefrontOBJMesh& objMesh)
    {
        Ref<Mesh> mesh = CreateRef<Mesh>();
        const WavefrontOBJ& parentOBJ = objMesh.GetParent();
        const auto& objVerticesData  = parentOBJ.GetVertices();
        const auto& objNormalsData   = parentOBJ.GetNormals();
        const auto& objTexCoordsData = parentOBJ.GetTexCoords();

        if (objVerticesData.empty())
        {
            RS_ERROR("[Mesh] No vertices found in OBJ file.");
            return nullptr;
        }
        if (objMesh.Faces.empty())
        {
            RS_ERROR("[Mesh] No faces found in OBJ file.");
            return nullptr;
        }
        mesh->m_HasNormals = !objNormalsData.empty();
        mesh->m_HasTexCoords = !objTexCoordsData.empty();

        std::unordered_map<MeshVertex, uint32_t> uniqueVertices;

        for (const auto& face : objMesh.Faces)
        {
            for (const auto& vertexIndex : face)
            {
                MeshVertex meshVertex;
                meshVertex.Position = objVerticesData[vertexIndex.Position];
                if (mesh->m_HasNormals)
                    meshVertex.Normal = objNormalsData[vertexIndex.Normal];
                if (mesh->m_HasTexCoords)
                    meshVertex.TexCoord = objTexCoordsData[vertexIndex.TexCoord];

                // Vertex deduplication
                if (uniqueVertices.count(meshVertex) == 0)
                {
                    uniqueVertices[meshVertex] = static_cast<uint32_t>(mesh->m_Vertices.size());
                    mesh->m_Vertices.push_back(meshVertex);
                }
                mesh->m_Indices.push_back(uniqueVertices[meshVertex]);
            }
        }

        return mesh;
    }

}
