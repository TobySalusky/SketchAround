#version 330 core
layout (location = 0) in vec2 pos;

uniform mat4 model;

void main()
{
    gl_Position = model * vec4(pos, 0.0f, 1.0f);
}