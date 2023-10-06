#pragma once

#include <string>
#include <sstream>
#include <iomanip>

#include <glm/glm.hpp>

class Util
{
public:
    static std::string ExtractFileNameFromPath(const std::string& filepath, bool includeExtension = false)
    {
        size_t lastSlash = filepath.find_last_of("/\\");
        lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
        std::string fileBaseName = filepath.substr(lastSlash, filepath.length());

        if (includeExtension)
            return fileBaseName;

        return StripFileExtension(fileBaseName);
    }

    static std::string StripFileExtension(const std::string& file)
    {
        size_t lastDot = file.find_last_of('.');
        size_t count = lastDot == std::string::npos ? file.length() : lastDot;
        return file.substr(0, count);
    }

    static std::string GenerateAnonymousName()
    {
        static int anonymousNameCount = 1;
        std::stringstream ss;
        ss << "Anonymous#" << std::setw(4) << std::setfill('0') << std::right << anonymousNameCount++;
        return ss.str();
    }

    static bool IsPointInRectangle(const glm::vec2& rectCenter, const glm::vec2& rectSize, const glm::vec2& point)
    {
        return (rectCenter.x - rectSize.x / 2 <= point.x && rectCenter.x + rectSize.x / 2 >= point.x &&
                rectCenter.y - rectSize.y / 2 <= point.y && rectCenter.y + rectSize.y / 2 >= point.y);
    }

    template <typename T>
    static T Clamp(T v1, T lower, T upper)
    {
        return glm::max(glm::min(v1, upper), lower);
    }
};
