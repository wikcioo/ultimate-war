#pragma once

#include <memory>
#include <string>
#include <variant>
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

    void Reload(const std::string& filepath = "");

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
    std::string m_FilePath;
    unsigned int m_ProgramID;
};

using UniformValue = std::variant<bool, int, float, glm::vec2, glm::vec3, glm::vec4, glm::mat4>;
struct ShaderData
{
    std::unordered_map<std::string, UniformValue> UniformMap;
    void Apply(const std::shared_ptr<Shader>& shader);
};
