#version 330 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

struct Material
{
    vec3  diffuse;    // Kd
    vec3  specular;   // Ks
    float shininess;  // Ns
};

uniform Material  material;
uniform bool      useTexture;
uniform sampler2D texture_diffuse1;

uniform vec3 lightDir;   // direccion HACIA la luz
uniform vec3 viewPos;

void main()
{
    // Color base: textura * Kd (Kd=1 en materiales con textura) o solo Kd
    vec3 albedo = material.diffuse;
    if (useTexture)
        albedo *= texture(texture_diffuse1, TexCoords).rgb;

    // Las paredes del diorama son planos: iluminar por ambos lados
    vec3 N = normalize(Normal);
    if (!gl_FrontFacing) N = -N;

    vec3 L = normalize(lightDir);
    vec3 V = normalize(viewPos - FragPos);
    vec3 H = normalize(L + V);

    vec3 ambient  = 0.40 * albedo;
    vec3 diffuse  = 0.75 * max(dot(N, L), 0.0) * albedo;
    vec3 specular = pow(max(dot(N, H), 0.0), material.shininess) * material.specular;

    FragColor = vec4(ambient + diffuse + specular, 1.0);
}
