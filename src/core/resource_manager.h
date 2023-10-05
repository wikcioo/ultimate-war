#pragma once

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

#include "graphics/font.h"
#include "graphics/shader.h"
#include "graphics/texture.h"

class ResourceManager
{
public:
    static void LoadFont(const std::string& name, const std::string& filepath);
    static void LoadShader(const std::string& name, const std::string& filepath);
    static void LoadTexture(const std::string& name, const std::string& filepath);

    static std::shared_ptr<Font> GetFont(const std::string& name);
    static std::shared_ptr<Shader> GetShader(const std::string& name);
    static std::shared_ptr<Texture2D> GetTexture(const std::string& name);

    static std::vector<std::string> GetAvailableFontNames();

private:
    static std::unordered_map<std::string, std::shared_ptr<Font>> m_FontCache;
    static std::unordered_map<std::string, std::shared_ptr<Shader>> m_ShaderCache;
    static std::unordered_map<std::string, std::shared_ptr<Texture2D>> m_TextureCache;
};
