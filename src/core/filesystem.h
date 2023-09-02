#pragma once

#include <string>
#include <fstream>

#include "logger.h"

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
};
