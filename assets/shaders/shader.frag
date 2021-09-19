#version 330 core

in vec4 vCol;
in vec3 normal;

out vec4 color;

struct DirectionalLight
{
    vec3 color;
    float ambientIntensity;
    vec3 direction;
    float diffuseIntensity;
};

uniform DirectionalLight directionalLight;


void main()
{
    vec4 ambientColor = vec4(directionalLight.color, 1.0f) * directionalLight.ambientIntensity;

    float diffuseFactor = max(0.0f, dot(normalize(normal), normalize(directionalLight.direction)));
    vec4 diffuseColor = vec4(directionalLight.color, 1.0f) * directionalLight.diffuseIntensity * diffuseFactor;

    color = vCol * (ambientColor + diffuseColor);
}