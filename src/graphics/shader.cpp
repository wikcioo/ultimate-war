#include "shader.h"

#include <fstream>
#include <sstream>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "core/logger.h"
#include "core/file_system.h"
#include "util/util.h"

Shader::Shader(const std::string& filepath)
    : m_FilePath(filepath)
{
    m_Name = Util::ExtractFileNameFromPath(filepath);

    std::string source = FileSystem::ReadFile(filepath);
    ShaderSourceMap shaderSources = Parse(source);
    m_ProgramID = Compile(shaderSources);
}

Shader::~Shader()
{
    glDeleteProgram(m_ProgramID);
}

void Shader::Bind() const
{
    glUseProgram(m_ProgramID);
}

void Shader::Unbind() const
{
    glUseProgram(0);
}

void Shader::Reload(const std::string& filepath)
{
    std::string source = FileSystem::ReadFile(filepath.empty() ? m_FilePath : filepath);
    ShaderSourceMap shaderSources = Parse(source);
    m_ProgramID = Compile(shaderSources);
}

std::string Shader::ShaderTypeToStr(unsigned int type)
{
    switch (type)
    {
        case GL_VERTEX_SHADER:   return "vertex";
        case GL_FRAGMENT_SHADER: return "fragment";
        case GL_GEOMETRY_SHADER: return "geometry";
    }

    return "unknown";
}

Shader::ShaderSourceMap Shader::Parse(const std::string& source)
{
    ShaderSourceMap ssm;

    enum class ShaderType
    {
        NONE = 0, VERTEX = 0x8B31, FRAGMENT = 0x8B30, GEOMETRY = 0x8DD9
    };

    ShaderType currentType = ShaderType::NONE;

    std::istringstream sourceStream(source);
    std::ostringstream builder;
    std::string line;

    while (std::getline(sourceStream, line))
    {
        if (line.find("#type") != std::string::npos)
        {
            if (currentType != ShaderType::NONE)
            {
                ssm[(unsigned int)currentType] = builder.str();
                builder.str("");
                builder.clear();
            }

            if (line.find("vertex") != std::string::npos)
                currentType = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                currentType = ShaderType::FRAGMENT;
            else if (line.find("geometry") != std::string::npos)
                currentType = ShaderType::GEOMETRY;
        }
        else
        {
            builder << line << '\n';
        }
    }

    ssm[(unsigned int)currentType] = builder.str();
    return ssm;
}

unsigned int Shader::Compile(const ShaderSourceMap& shaderSources)
{
    std::vector<unsigned int> compiledShaderIDs;

    for (auto [type, source] : shaderSources)
    {
        unsigned int shader = glCreateShader(type);
        const char* src = source.c_str();
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);

        int compiled;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled)
        {
            int length;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

            std::vector<char> message;
            message.reserve(length);

            glGetShaderInfoLog(shader, length, nullptr, &message[0]);
            LOG_ERROR("Shader: name '{0}', type '{1}' failed to compile!\n{2}",
                       m_Name, ShaderTypeToStr(type), message.data());
        }

        compiledShaderIDs.push_back(shader);
    }

    unsigned int program = glCreateProgram();

    for (unsigned int id : compiledShaderIDs)
        glAttachShader(program, id);

    glLinkProgram(program);

    int linked;
    glGetProgramiv(program, GL_LINK_STATUS, &linked);
    if (!linked)
    {
        int length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);

        std::vector<char> message;
        message.reserve(length);

        glGetProgramInfoLog(program, length, nullptr, &message[0]);
        LOG_ERROR("Shader: name '{0}', type 'program' failed to link!\n{1}",
                   m_Name, message.data());
    }

    for (unsigned int id : compiledShaderIDs)
        glDeleteShader(id);

    return program;
}

void Shader::SetBool(const std::string& name, bool value)
{
    int location = glGetUniformLocation(m_ProgramID, name.c_str());
    glUniform1i(location, value);
}

void Shader::SetInt(const std::string& name, int value)
{
    int location = glGetUniformLocation(m_ProgramID, name.c_str());
    glUniform1i(location, value);
}

void Shader::SetFloat(const std::string& name, float value)
{
    int location = glGetUniformLocation(m_ProgramID, name.c_str());
    glUniform1f(location, value);
}

void Shader::SetFloat2(const std::string& name, const glm::vec2& value)
{
    int location = glGetUniformLocation(m_ProgramID, name.c_str());
    glUniform2f(location, value.x, value.y);
}

void Shader::SetFloat3(const std::string& name, const glm::vec3& value)
{
    int location = glGetUniformLocation(m_ProgramID, name.c_str());
    glUniform3f(location, value.x, value.y, value.z);
}

void Shader::SetFloat4(const std::string& name, const glm::vec4& value)
{
    int location = glGetUniformLocation(m_ProgramID, name.c_str());
    glUniform4f(location, value.x, value.y, value.z, value.w);
}

void Shader::SetMat4(const std::string& name, const glm::mat4& value)
{
    int location = glGetUniformLocation(m_ProgramID, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void ShaderData::Apply(const std::shared_ptr<Shader>& shader)
{
    for (const auto& pair : UniformMap)
    {
        const std::string& key = pair.first;
        const UniformValue& value = pair.second;

        if (std::holds_alternative<bool>(value))
            shader->SetBool(key, std::get<bool>(value));
        else if (std::holds_alternative<int>(value))
            shader->SetInt(key, std::get<int>(value));
        else if (std::holds_alternative<float>(value))
            shader->SetFloat(key, std::get<float>(value));
        else if (std::holds_alternative<glm::vec2>(value))
            shader->SetFloat2(key, std::get<glm::vec2>(value));
        else if (std::holds_alternative<glm::vec3>(value))
            shader->SetFloat3(key, std::get<glm::vec3>(value));
        else if (std::holds_alternative<glm::vec4>(value))
            shader->SetFloat4(key, std::get<glm::vec4>(value));
        else if (std::holds_alternative<glm::mat4>(value))
            shader->SetMat4(key, std::get<glm::mat4>(value));
        else
            LOG_WARN("Cannot apply uniform to shader - unknown type");
    }
}
