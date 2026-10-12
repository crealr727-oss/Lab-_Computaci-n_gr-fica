#version 330 core

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 color;

uniform vec3 viewPos;
uniform Material material;

uniform Light light;    // Luz 1: Sol
uniform Light light2;   // Luz 2: Luna
uniform int luzActiva;  // 1 = Sol, 2 = Luna

uniform sampler2D texture_diffuse1;
uniform int hasTexture;

// Calcula Phong (ambiental + difusa + especular) para una luz
vec3 CalcLuz(Light L, vec3 norm, vec3 viewDir, vec3 base)
{
    // Ambiental
    vec3 ambient = L.ambient * material.ambient * base;

    // Difusa
    vec3 lightDir = normalize(L.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = L.diffuse * diff * material.diffuse * base;

    // Especular
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = L.specular * spec * material.specular;

    return ambient + diffuse + specular;
}

void main()
{
    // Color base: textura del modelo (o blanco si la malla no tiene textura)
    vec3 base = (hasTexture == 1) ? texture(texture_diffuse1, TexCoords).rgb : vec3(1.0);

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result;
    if (luzActiva == 1)
        result = CalcLuz(light, norm, viewDir, base);
    else
        result = CalcLuz(light2, norm, viewDir, base);

    color = vec4(result, 1.0);
}
