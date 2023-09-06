#pragma once

#include <string>
#include <memory>
#include <unordered_map>

#include "graphics/shader.h"
#include "graphics/texture.h"

class ResourceManager
{
public:
    static void LoadShader(const std::string& name, const std::string& filepath);
    static void LoadTexture(const std::string& name, const std::string& filepath);

    static std::shared_ptr<Shader> GetShader(const std::string& name);
    static std::shared_ptr<Texture2D> GetTexture(const std::string& name);

private:
    static std::unordered_map<std::string, std::shared_ptr<Shader>> m_ShaderCache;
    static std::unordered_map<std::string, std::shared_ptr<Texture2D>> m_TextureCache;
};
