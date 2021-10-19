#version 330 core

in vec4 vCol;
in vec3 normal;
in vec3 fragPos;

out vec4 color;

struct DirectionalLight
{
    vec3 color;
    float ambientIntensity;
    vec3 direction;
    float diffuseIntensity;
};

struct Material {
    float specularIntensity;
    float shininess;
};

uniform DirectionalLight directionalLight;

uniform Material material;
uniform vec3 cameraPosition;

void main()
{
    vec4 ambientColor = vec4(directionalLight.color, 1.0f) * directionalLight.ambientIntensity;

    float diffuseFactor = max(0.0f, dot(normalize(normal), normalize(directionalLight.direction)));
    vec4 diffuseColor = vec4(directionalLight.color, 1.0f) * directionalLight.diffuseIntensity * diffuseFactor;

    vec4 specularColor = vec4(0.0f, 0.0f, 0.0f, 0.0f);

    if (diffuseFactor > 0.0f) {
        vec3 fragToCameraDir = normalize(cameraPosition - fragPos);
        vec3 relfectionDir = normalize(reflect(directionalLight.direction, normalize(normal)));

        float specularFactor = dot(fragToCameraDir, relfectionDir);
        if (specularFactor > 0.0f) {
            specularFactor = pow(specularFactor, material.shininess);
            specularColor = vec4(directionalLight.color * material.specularIntensity * specularFactor, 1.0f);
        }
    }

    color = vCol * (ambientColor + diffuseColor + specularColor);
}