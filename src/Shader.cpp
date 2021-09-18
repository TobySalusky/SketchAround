//
// Created by Tobiathan on 9/18/21.
//
#include <iostream>
#include <glew.h>
#include <fstream>

#include "Shader.h"

Shader::Shader(const char* vertexShaderSource, const char* fragmentShaderSource) {

    // Shaders

    // Build and compile our shader program
    // Vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    // Check for compile time errors
    GLint success;
    GLchar infoLog[512];

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    // Check for compile time errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Link shaders
    ID = glCreateProgram();
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);

    // Check for linking errors
    glGetProgramiv(ID, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog(ID, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::~Shader() = default;

void Shader::Enable() const {
    glUseProgram(ID);
}

Uniform Shader::GenUniform(const char *identifier) const {
    return {glGetUniformLocation(ID, identifier)};
}

std::string Shader::ReadFile(const char *path) {
    std:: string content;
    std::ifstream fileStream(path, std::ios::in);

    if (!fileStream.is_open()) {
        printf("Failed to read %s! File doesn't exist!", path);
    }

    std::string line;
    while (!fileStream.eof())
    {
        std::getline(fileStream, line);
        content.append(line + '\n');
    }
    fileStream.close();

    return content;
}

Shader Shader::Read(const char *vertexShaderPath, const char *fragmentShaderPath) {
    return Shader(ReadFile(vertexShaderPath).c_str(), ReadFile(fragmentShaderPath).c_str());
}
