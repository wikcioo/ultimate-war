#pragma once

#include <string>
#include <unordered_map>

#include <glm/glm.hpp>

class Shader
{
    using ShaderSourceMap = std::unordered_map<unsigned int, std::string>;
public:
    Shader(const std::string& filepath);
    ~Shader();

    void Bind() const;
    void Unbind() const;

    void SetBool(const std::string& name, bool value);
    void SetInt(const std::string& name, int value);
    void SetFloat(const std::string& name, float value);
    void SetFloat2(const std::string& name, const glm::vec2& value);
    void SetFloat3(const std::string& name, const glm::vec3& value);
    void SetFloat4(const std::string& name, const glm::vec4& value);
    void SetMat4(const std::string& name, const glm::mat4& value);

private:
    std::string ShaderTypeToStr(unsigned int type);
    ShaderSourceMap Parse(const std::string& source);
    unsigned int Compile(const ShaderSourceMap& shaderSources);

private:
    std::string m_Name;
    unsigned int m_ProgramID;
};
