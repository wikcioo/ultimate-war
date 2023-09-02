#pragma once

#include <string>

class Util
{
public:
    static std::string ExtractFileNameFromPath(const std::string& filepath)
    {
        size_t lastSlash = filepath.find_last_of("/\\");
        lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
        size_t lastDot = filepath.find_last_of('.');
        size_t count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
        return filepath.substr(lastSlash, count);
    }
};
