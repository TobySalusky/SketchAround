#version 330 core
layout (location = 0) in vec2 pos;
layout (location = 1) in vec4 col;

out vec4 vCol;

uniform mat4 model;

void main()
{
    gl_Position = model * vec4(pos, 0.0f, 1.0f);
    vCol = col;
}