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
        #ifdef RS_DEBUG
        uint32_t dupeVerticesRemoved = 0;
        #endif

        for (const auto& face : objMesh.Faces)
        {
            for (const auto& [Position, TexCoord, Normal] : face)
            {
                MeshVertex meshVertex;
                meshVertex.Position = objVerticesData[Position];
                if (m_HasNormals)
                    meshVertex.Normal = objNormalsData[Normal];
                if (m_HasTexCoords)
                    meshVertex.TexCoord = objTexCoordsData[TexCoord];

                // Vertex deduplication
                if (uniqueVertices.count(meshVertex) == 0)
                {
                    uniqueVertices[meshVertex] = static_cast<uint32_t>(m_Vertices.size());
                    m_Vertices.push_back(meshVertex);
                }
                #ifdef RS_DEBUG
                else
                {
                    dupeVerticesRemoved++;
                }
                #endif
                m_Indices.push_back(uniqueVertices[meshVertex]);
            }
        }
        #ifdef RS_DEBUG
        RS_INFO("[Mesh] Removed %u duplicate vertices from %s", dupeVerticesRemoved, objMesh.Name.c_str());
        #endif
    }

}
