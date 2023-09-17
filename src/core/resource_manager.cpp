#include "resource_manager.h"

#include <stb/stb_image.h>

#include "logger.h"

std::unordered_map<std::string, std::shared_ptr<Shader>> ResourceManager::m_ShaderCache;
std::unordered_map<std::string, std::shared_ptr<Texture2D>> ResourceManager::m_TextureCache;

void ResourceManager::LoadShader(const std::string& name, const std::string& filepath)
{
    if (m_ShaderCache.find(name) != m_ShaderCache.end())
    {
        LOG_ERROR("ResourceManager: Shader with name {0} already in cache", name);
        return;
    }

    std::shared_ptr<Shader> shader = std::make_shared<Shader>(filepath);
    m_ShaderCache[name] = shader;
}

void ResourceManager::LoadTexture(const std::string& name, const std::string& filepath)
{
    if (m_TextureCache.find(name) != m_TextureCache.end())
    {
        LOG_ERROR("ResourceManager: Texture with name {0} already in cache", name);
        return;
    }

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(1);
    unsigned char *data = stbi_load(filepath.c_str(), &width, &height, &nrChannels, 0);

    TextureData textureData = {
        .Size={width, height},
        .Data=data,
        .NrChannels=(unsigned int)nrChannels,
    };
    std::shared_ptr<Texture2D> texture = std::make_shared<Texture2D>(textureData);

    stbi_image_free(data);
    m_TextureCache[name] = texture;
}

std::shared_ptr<Shader> ResourceManager::GetShader(const std::string& name)
{
    if (m_ShaderCache.find(name) == m_ShaderCache.end())
    {
        LOG_ERROR("ResourceManager: Unknown shader with name {0}", name);
        return nullptr;
    }

    return m_ShaderCache[name];
}

std::shared_ptr<Texture2D> ResourceManager::GetTexture(const std::string& name)
{
    if (m_TextureCache.find(name) == m_TextureCache.end())
    {
        LOG_ERROR("ResourceManager: Unknown texture with name {0}", name);
        return nullptr;
    }

    return m_TextureCache[name];
}
