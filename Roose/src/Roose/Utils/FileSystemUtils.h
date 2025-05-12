#pragma once

#include <string>

namespace Roose {

    class FileSystemUtils
    {
    public:
        /**
         * @brief Extract the file extension from a file path.
         * @param filePath The file path as a string.
         * @return The file extension as a string.
         */
        static std::string GetFileExtension(const std::string& filePath);
    
        /**
         * @brief Extract the file name from a file path.
         * @param filePath The file path as a string.
         * @return The file name as a string.
         */
        static std::string GetFileName(const std::string& filePath);

        /**
         * @brief Extract the file name without the extension from a file path.
         * @param filePath The file path as a string.
         * @return The file name without the extension as a string.
         */
        static std::string GetFileNameWithoutExtension(const std::string& filePath);

        /**
         * @brief Extract the directory from a file path.
         * @param filePath The file path as a string.
         * @return The directory path as a string.
         */
        static std::string GetDirectory(const std::string& filePath);

        /**
         * @brief Convert a relative path to an absolute path.
         * @param relativePath The relative file path as a string.
         * @return The absolute file path as a string.
         */
        static std::string ToAbsolutePath(const std::string& relativePath);

        /**
         * @brief Join multiple paths into a single path.
         * @param basePath The base path as a string.
         * @param relativePath The relative path to append.
         * @return The combined path as a string.
         */
        static std::string JoinPaths(const std::string& basePath, const std::string& relativePath);

        /**
         * @brief Verify if a file exists given a file path.
         * @param filePath The file path as a string.
         * @return True if the file exists, false otherwise.
         */
        static bool FileExists(const std::string& filePath);

        /**
         * @brief Read the contents of a file into a string.
         * @param filePath The file path as a string.
         * @return The file contents as a string.
         */
        static std::string ReadFileAsString(const std::string& filePath);
    };

}
