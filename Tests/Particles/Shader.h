#pragma once

#include <glad/glad.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <array>
#include <vector>

class Shader
{
public:
    ~Shader()
    {
        glDeleteProgram(ID);
    }

    void InitializeFromPath(const char* vertexPath, const char* fragmentPath)
    {
        InitializeFromSource(LoadShaderFromFile(vertexPath).c_str(), LoadShaderFromFile(fragmentPath).c_str());
    }

    void InitializeFromPath(const char* vertexPath, const char* fragmentPath, const std::vector<std::string>& varyings)
    {
        InitializeFromSource(LoadShaderFromFile(vertexPath).c_str(), LoadShaderFromFile(fragmentPath).c_str(), varyings);
    }

    void InitializeFromSource(const char* vertexSource, const char* fragmentSource)
    {
        GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource);
        GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

        ID = glCreateProgram();
        glAttachShader(ID, vertexShader);
        glAttachShader(ID, fragmentShader);
        glLinkProgram(ID);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    void InitializeFromSource(const char* vertexSource, const char* fragmentSource, const std::vector<std::string>& varyings)
    {
        std::vector<const char*> varyingsCStr;
        for (const std::string& varying : varyings)
        {
            varyingsCStr.push_back(varying.c_str());
        }

        GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource);
        GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

        ID = glCreateProgram();
        glAttachShader(ID, vertexShader);
        glAttachShader(ID, fragmentShader);
        glTransformFeedbackVaryings(ID, static_cast<GLsizei>(varyingsCStr.size()), varyingsCStr.data(), GL_INTERLEAVED_ATTRIBS);
        glLinkProgram(ID);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    static std::string LoadShaderFromFile(const char* filePath)
    {
        std::ifstream shaderFile;
        shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try
        {
            //Open file
            shaderFile.open(filePath, std::ifstream::in);
            std::stringstream vShaderStream;

            //Read file's buffer contents into streams
            vShaderStream << shaderFile.rdbuf();

            //Close file
            shaderFile.close();

            return vShaderStream.str();
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
        }

        return {};
    }

    void Use() const
    {
        glUseProgram(ID);
    }

    [[nodiscard]] GLuint GetID() const
    {
        return ID;
    }

    static GLuint CompileShader(GLenum type, const char* source)
    {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, nullptr);
        glCompileShader(shader);

        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (!success)
        {
            std::array<char, 1024> infoLog{};
            glGetShaderInfoLog(shader, 512, nullptr, infoLog.data());
            std::cerr << "Shader compilation error: " << infoLog.data() << std::endl;
        }

        return shader;
    }

    //Functions to set values on the shader
    void SetBool(const char* name, bool value) const
    {
        glUniform1i(glGetUniformLocation(ID, name), static_cast<int>(value));
    }

    void SetInt(const char* name, int value) const
    {
        glUniform1i(glGetUniformLocation(ID, name), value);
    }

    void SetFloat(const char* name, float value) const
    {
        glUniform1f(glGetUniformLocation(ID, name), value);
    }

    void SetVec2(const char* name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(ID, name), x, y);
    }

    void SetVec3(const char* name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(ID, name), x, y, z);
    }

    void SetVec4(const char* name, float x, float y, float z, float w) const
    {
        glUniform4f(glGetUniformLocation(ID, name), x, y, z, w);
    }

private:
    GLuint ID;
};