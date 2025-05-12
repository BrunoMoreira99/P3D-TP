#include "rspch.h"
#include "Roose/Importers/WavefrontOBJ.h"
#include "Roose/Utils/FileSystemUtils.h"

#include <fstream>

namespace Roose {

    bool WavefrontOBJ::Load(const std::string& filepath)
    {
        std::ifstream file(filepath);
        if (!file)
        {
            RS_ERROR("Failed to open OBJ file: %s", filepath.c_str());
            return false;
        }

        const std::string directory = FileSystemUtils::GetDirectory(filepath);

        bool notifiedAboutNonTriangles = false;
        bool notifiedAboutInvalidFaces = false;
        WavefrontOBJMesh* currentMesh = nullptr;

        std::string line;
        while (std::getline(file, line))
        {
            std::istringstream iss(line);
            std::string prefix;
            iss >> prefix;

            if (prefix == "mtllib")
            {
                iss >> m_MaterialFilePath;
                m_MaterialFilePath = FileSystemUtils::JoinPaths(directory, m_MaterialFilePath);
            }
            else if (prefix == "o" || prefix == "g")
            {
                m_Meshes.emplace_back(this);
                currentMesh = &m_Meshes.back();
                iss >> currentMesh->Name;
            }
            else if (prefix == "usemtl")
            {
                if (!currentMesh)
                {
                    m_Meshes.emplace_back(this);
                    currentMesh = &m_Meshes.back();
                }
                iss >> currentMesh->MaterialName;
            }
            else if (prefix == "v")
            {
                std::string x, y, z;
                iss >> x >> y >> z;
                m_Vertices.emplace_back(std::stof(x), std::stof(y), std::stof(z));
            }
            else if (prefix == "vn")
            {
                std::string x, y, z;
                iss >> x >> y >> z;
                m_Normals.emplace_back(std::stof(x), std::stof(y), std::stof(z));
            }
            else if (prefix == "vt")
            {
                std::string x, y;
                iss >> x >> y;
                m_TexCoords.emplace_back(std::stof(x), std::stof(y));
            }
            else if (prefix == "f")
            {
                if (!currentMesh)
                {
                    m_Meshes.emplace_back(this);
                    currentMesh = &m_Meshes.back();
                }

                std::vector<std::string> vertexTokens;
                std::string vertexData;

                while (iss >> vertexData)
                {
                    vertexTokens.push_back(vertexData);
                }

                // Check for non-triangular faces
                if (vertexTokens.size() != 3)
                {
                    if (!notifiedAboutNonTriangles && vertexTokens.size() > 3)
                    {
                        RS_ERROR("Non-triangular face found in OBJ file: %s\n"
                                 "Currently, Roose only supports Triangles. "
                                 "Any non-triangular face will be skipped.", filepath.c_str());
                        notifiedAboutNonTriangles = true;
                    }
                    if (!notifiedAboutInvalidFaces && vertexTokens.size() < 3)
                    {
                        RS_ERROR("Invalid face definition in OBJ file: %s\n"
                                 "A face must have at least 3 vertices.", filepath.c_str());
                        notifiedAboutInvalidFaces = true;
                    }
                    continue;
                }

                std::vector<WavefrontOBJFaceVertexIndex> face;
                for (const std::string& token : vertexTokens)
                {
                    std::string parsed = token;
                    std::replace(parsed.begin(), parsed.end(), '/', ' ');
                    std::istringstream vertexStream(parsed);

                    WavefrontOBJFaceVertexIndex vi;
                    vertexStream >> vi.Position >> vi.TexCoord >> vi.Normal;
                    vi.Position -= 1; // OBJ indices are 1-based
                    vi.TexCoord -= 1;
                    vi.Normal -= 1;

                    face.push_back(vi);
                }

                currentMesh->Faces.push_back(face);
            }
        }

        if (m_Meshes.empty())
        {
            RS_ERROR("No meshes found in OBJ file: %s", filepath.c_str());
            return false;
        }
        // Sort the meshes by material name to ensure that all meshes using the same material are grouped together
        std::sort(m_Meshes.begin(), m_Meshes.end(), [](const WavefrontOBJMesh& a, const WavefrontOBJMesh& b)
        {
            return a.MaterialName < b.MaterialName;
        });

        return true;
    }

}
