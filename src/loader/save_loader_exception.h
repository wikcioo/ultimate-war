#pragma once

#include <string>
#include <exception>

class SaveLoaderException : public std::exception
{
public:
    explicit SaveLoaderException(const std::string& message) : m_ErrorMessage(message) {}

    const char* what() const noexcept override
    {
        return m_ErrorMessage.c_str();
    }

private:
    std::string m_ErrorMessage;
};
