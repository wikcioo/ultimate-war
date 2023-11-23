#pragma once

#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <functional>

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>

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

    static std::string ReplaceChar(const std::string& str, char oldChar, char newChar)
    {
        std::string s = "";
        for (size_t i = 0; i < str.size(); i++)
            s += str[i] == oldChar ? newChar : str[i];
        return s;
    }

    static std::string StrToUpper(const std::string& str)
    {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::toupper);
        return result;
    }

    static std::string StrToLower(const std::string& str)
    {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
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

    template <typename T, typename P>
    static void RemoveElementsFromContainerWithCondition(T& container, std::function<bool(P)> func)
    {
        container.erase(std::remove_if(container.begin(), container.end(), func), container.end());
    }

    static glm::vec3 GetRandomColor()
    {
        return glm::linearRand(glm::vec3(0.0f), glm::vec3(1.0f));
    }

    static void RemoveCRLF(std::string& input)
    {
        input.erase(std::remove(input.begin(), input.end(), '\r'), input.end());
        input.erase(std::remove(input.begin(), input.end(), '\n'), input.end());
    }
};
