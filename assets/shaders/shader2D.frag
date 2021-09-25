#version 330 core

out vec4 color;

uniform vec4 color2D;


void main()
{
    color = color2D;
}