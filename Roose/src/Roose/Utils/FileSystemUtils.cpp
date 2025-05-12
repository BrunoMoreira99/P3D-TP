#include "rspch.h"
#include "Roose/Utils/FileSystemUtils.h"

#include <filesystem>
#include <fstream>

namespace Roose {

    std::string FileSystemUtils::GetFileExtension(const std::string& filePath)
    {
        return std::filesystem::path(filePath).extension().string();
    }

    std::string FileSystemUtils::GetFileName(const std::string& filePath)
    {
        return std::filesystem::path(filePath).filename().string();
    }

    std::string FileSystemUtils::GetDirectory(const std::string& filePath)
    {
        return std::filesystem::path(filePath).parent_path().string();
    }

    bool FileSystemUtils::FileExists(const std::string& filePath)
    {
        return std::filesystem::exists(filePath);
    }

    std::string FileSystemUtils::ToAbsolutePath(const std::string& relativePath)
    {
        return std::filesystem::absolute(relativePath).string();
    }

    std::string FileSystemUtils::JoinPaths(const std::string& basePath, const std::string& relativePath)
    {
        return (std::filesystem::path(basePath) / std::filesystem::path(relativePath)).string();
    }

    std::string FileSystemUtils::ReadFileAsString(const std::string& filePath)
    {
        std::string result;
        std::ifstream in(filePath, std::ios::in | std::ios::binary);
        if (in)
        {
            in.seekg(0, std::ios::end);
            result.resize((size_t)in.tellg());
            in.seekg(0, std::ios::beg);
            in.read(result.data(), result.size());
            in.close();
        }
        else
        {
            RS_ERROR("Could not open file '%s'", filePath.c_str());
        }

        return result;
    }

}
