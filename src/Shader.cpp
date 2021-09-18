//
// Created by Tobiathan on 9/18/21.
//
#include <iostream>
#include <glew.h>

#include "Shader.h"

Shader::Shader() {

    // Shaders
    const char* vertexShaderSource = R"glsl(
#version 330 core
layout (location = 0) in vec3 pos;

out vec4 vCol;

uniform mat4 model;
uniform mat4 projection;

void main()
{
    gl_Position = projection * model * vec4(pos, 1.0f);
    vCol = vec4(clamp(pos, 0.0f, 1.0f), 1.0f);
}
)glsl";
    const char* fragmentShaderSource = R"glsl(
#version 330 core
in vec4 vCol;
out vec4 color;
void main()
{
    color = vCol;
}
)glsl";


    // Build and compile our shader program
    // Vertex shader
    GLuint vertexShader = glCreateShader( GL_VERTEX_SHADER );
    glShaderSource( vertexShader, 1, &vertexShaderSource, NULL );
    glCompileShader( vertexShader );

    // Check for compile time errors
    GLint success;
    GLchar infoLog[512];

    glGetShaderiv( vertexShader, GL_COMPILE_STATUS, &success );
    if ( !success )
    {
        glGetShaderInfoLog( vertexShader, 512, NULL, infoLog );
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Fragment shader
    GLuint fragmentShader = glCreateShader( GL_FRAGMENT_SHADER );
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL );
    glCompileShader(fragmentShader);

    // Check for compile time errors
    glGetShaderiv( fragmentShader, GL_COMPILE_STATUS, &success );

    if ( !success )
    {
        glGetShaderInfoLog( fragmentShader, 512, NULL, infoLog );
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Link shaders
    ID = glCreateProgram( );
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    glLinkProgram(ID);

    // Check for linking errors
    glGetProgramiv(ID, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::~Shader() = default;

void Shader::Enable() {
    glUseProgram(ID);
}

Uniform Shader::GenUniform(const char *identifier) {
    return {glGetUniformLocation(ID, identifier)};
}
