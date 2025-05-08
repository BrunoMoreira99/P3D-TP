#include "rspch.h"
#include "Roose/Importers/WavefrontMTL.h"

#include <fstream>

namespace Roose {

    bool WavefrontMTL::Load(const std::string& filepath)
    {
        std::ifstream file(filepath);
        if (!file.is_open())
        {
            RS_ERROR("Failed to open MTL file: %s", filepath.c_str());
            return false;
        }

        Material* currentMaterial = nullptr;

        std::string line;
        while (std::getline(file, line))
        {
            std::istringstream iss(line);
            std::string prefix;
            iss >> prefix;

            if (prefix == "newmtl")
            {
                std::string materialName;
                iss >> materialName;
                m_Materials[materialName] = Material();
                currentMaterial = &m_Materials[materialName];
            }
            else if (currentMaterial)
            {
                if (prefix == "Ka")
                {
                    std::string x, y, z;
                    iss >> x >> y >> z;
                    currentMaterial->Ka = { std::stof(x), std::stof(y), std::stof(z) };
                }
                else if (prefix == "Kd")
                {
                    std::string x, y, z;
                    iss >> x >> y >> z;
                    currentMaterial->Kd = { std::stof(x), std::stof(y), std::stof(z) };
                }
                else if (prefix == "Ks")
                {
                    std::string x, y, z;
                    iss >> x >> y >> z;
                    currentMaterial->Ks = { std::stof(x), std::stof(y), std::stof(z) };
                }
                else if (prefix == "Ns")
                {
                    std::string ns;
                    iss >> ns;
                    currentMaterial->Ns = std::stof(ns);
                }
                else if (prefix == "map_Kd")
                {
                    iss >> currentMaterial->Texture;
                }
            }
        }

        return true;
    }

    const WavefrontMTL::Material* WavefrontMTL::GetMaterial(const std::string& name) const
    {
        const auto it = m_Materials.find(name);
        return it != m_Materials.end() ? &it->second : nullptr;
    }

}
