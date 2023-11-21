#pragma once

#include <string>
#include <fstream>
#include <filesystem>

#include "logger.h"
#include "util/util.h"

class FileSystem
{
public:
    static std::string ReadFile(const std::string& filepath)
    {
        std::string str;

        std::ifstream ifs(filepath, std::ios::in | std::ios::binary);
        if (ifs)
        {
            ifs.seekg(0, std::ios::end);
            str.resize(ifs.tellg());
            ifs.seekg(0, std::ios::beg);
            ifs.read(&str[0], str.size());
            ifs.close();
        }
        else
        {
            LOG_ERROR("FileSystem: Could not open file {0}", filepath);
        }

        return str;
    }

    static std::vector<std::string> GetAllFilesInDirectory(const std::string& path, bool includeExtension = true)
    {
        std::vector<std::string> files;
        for (auto& entry : std::filesystem::directory_iterator(path))
        {
            files.emplace_back(Util::ExtractFileNameFromPath(entry.path().string(), includeExtension));
        }

        return files;
    }

    static std::vector<std::string> GetFilesInDirectoryWithExtension(const std::string& directory, const std::string& extension)
    {
        std::vector<std::string> files = FileSystem::GetAllFilesInDirectory(directory);

        // Remove all entries in files vector which do not end with extension
        files.erase(std::remove_if(files.begin(), files.end(), [extension](const std::string& s) {
            if (s.length() <= extension.length()) return true;
            return (s.compare(s.length() - extension.length(), extension.length(), extension) != 0);
        }), files.end());

        for (auto& file : files)
            file = Util::StripFileExtension(file);

        return files;
    }
};
