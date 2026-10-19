#version 330 core

#define NUMBER_OF_POINT_LIGHTS 3
#define NUMBER_OF_SPOT_LIGHTS  9

// Si las luces del auto (o el foco de la farola) brillan en una parte distinta
// de la textura, cambia este valor a 0 (depende de como Model.h invierte las UV).
#define TEXCOORD_V_FLIPPED 1

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct DirLight
{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight
{
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight
{
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 color;

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NUMBER_OF_POINT_LIGHTS];
uniform SpotLight spotLights[NUMBER_OF_SPOT_LIGHTS];
uniform Material material;
uniform int transparency;

// Tipo de objeto que se esta dibujando: 0 = normal, 1 = auto, 2 = farola, 3 = Sol / Luna (emiten luz propia)
uniform int objectType;

// Brillo propio de las partes luminosas (0 = apagado, 1 = encendido)
uniform float emitCabin;    // ventanas del auto (luz interior)
uniform float emitHead;     // faros delanteros
uniform float emitRear;     // direccionales traseras
uniform float emitLamp;     // foco de la farola
uniform float emitBody;     // brillo del Sol / la Luna (objectType 3)

// Function prototypes
vec3 CalcDirLight( DirLight light, vec3 normal, vec3 viewDir );
vec3 CalcPointLight( PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir );
vec3 CalcSpotLight( SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir );

// Las texturas del auto y de la farola son atlas de 4x4 casillas.
// Devuelve (columna, fila) de la casilla que corresponde a este fragmento.
ivec2 AtlasTile()
{
#if TEXCOORD_V_FLIPPED == 1
    float v = TexCoords.y;
#else
    float v = 1.0 - TexCoords.y;
#endif
    ivec2 t = ivec2( floor( TexCoords.x * 4.0 ), floor( v * 4.0 ) );
    return clamp( t, ivec2( 0 ), ivec2( 3 ) );
}

vec3 CalcEmission( vec3 texColor )
{
    ivec2 tile = AtlasTile();

    if ( objectType == 1 )
    {
        // Auto: (1,0) vidrios | (0,1) faros | (3,1) luces traseras
        if ( tile == ivec2( 1, 0 ) )
            return texColor * vec3( 1.5, 1.2, 0.7 ) * 0.9 * emitCabin;
        if ( tile == ivec2( 0, 1 ) )
            return texColor * vec3( 1.2, 1.15, 1.0 ) * 1.4 * emitHead;
        if ( tile == ivec2( 3, 1 ) )
            return vec3( 1.0, 0.42, 0.05 ) * 1.3 * emitRear;
    }
    else if ( objectType == 2 )
    {
        // Farola: (2,0) foco
        if ( tile == ivec2( 2, 0 ) )
            return texColor * 1.6 * emitLamp;
    }
    return vec3( 0.0 );
}

void main( )
{
    // Sol y Luna: se muestran con su textura completa, sin calculo de iluminacion
    if ( objectType == 3 )
    {
        vec4 t = texture( material.diffuse, TexCoords );
        color = vec4( t.rgb * emitBody, t.a );
        return;
    }

    // Properties
    vec3 norm = normalize( Normal );
    vec3 viewDir = normalize( viewPos - FragPos );

    // Directional lighting (luz ambiental tipo luna)
    vec3 result = CalcDirLight( dirLight, norm, viewDir );

    // Point lights (luz interior y direccionales traseras)
    for ( int i = 0; i < NUMBER_OF_POINT_LIGHTS; i++ )
    {
        result += CalcPointLight( pointLights[i], norm, FragPos, viewDir );
    }

    // Spot lights (faros, farolas y linterna)
    for ( int i = 0; i < NUMBER_OF_SPOT_LIGHTS; i++ )
    {
        result += CalcSpotLight( spotLights[i], norm, FragPos, viewDir );
    }

    vec4 texel = texture( material.diffuse, TexCoords );
    result += CalcEmission( texel.rgb );

    float alpha = texel.a;
    color = vec4( result, alpha );
    if(color.a < 0.1 && transparency == 1)
        discard;
}

// Calculates the color when using a directional light.
vec3 CalcDirLight( DirLight light, vec3 normal, vec3 viewDir )
{
    vec3 lightDir = normalize( -light.direction );

    // Diffuse shading
    float diff = max( dot( normal, lightDir ), 0.0 );

    // Specular shading
    vec3 reflectDir = reflect( -lightDir, normal );
    float spec = pow( max( dot( viewDir, reflectDir ), 0.0 ), material.shininess );

    // Combine results
    vec3 ambient = light.ambient * vec3( texture( material.diffuse, TexCoords ) );
    vec3 diffuse = light.diffuse * diff * vec3( texture( material.diffuse, TexCoords ) );
    vec3 specular = light.specular * spec * vec3( texture( material.specular, TexCoords ) );

    return ( ambient + diffuse + specular );
}

// Calculates the color when using a point light.
vec3 CalcPointLight( PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir )
{
    vec3 lightDir = normalize( light.position - fragPos );

    // Diffuse shading
    float diff = max( dot( normal, lightDir ), 0.0 );

    // Specular shading
    vec3 reflectDir = reflect( -lightDir, normal );
    float spec = pow( max( dot( viewDir, reflectDir ), 0.0 ), material.shininess );

    // Attenuation
    float distance = length( light.position - fragPos );
    float attenuation = 1.0f / ( light.constant + light.linear * distance + light.quadratic * ( distance * distance ) );

    // Combine results
    vec3 ambient = light.ambient * vec3( texture( material.diffuse, TexCoords ) );
    vec3 diffuse = light.diffuse * diff * vec3( texture( material.diffuse, TexCoords ) );
    vec3 specular = light.specular * spec * vec3( texture( material.specular, TexCoords ) );

    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

    return ( ambient + diffuse + specular );
}

// Calculates the color when using a spot light.
vec3 CalcSpotLight( SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir )
{
    vec3 lightDir = normalize( light.position - fragPos );

    // Diffuse shading
    float diff = max( dot( normal, lightDir ), 0.0 );

    // Specular shading
    vec3 reflectDir = reflect( -lightDir, normal );
    float spec = pow( max( dot( viewDir, reflectDir ), 0.0 ), material.shininess );

    // Attenuation
    float distance = length( light.position - fragPos );
    float attenuation = 1.0f / ( light.constant + light.linear * distance + light.quadratic * ( distance * distance ) );

    // Spotlight intensity
    float theta = dot( lightDir, normalize( -light.direction ) );
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp( ( theta - light.outerCutOff ) / epsilon, 0.0, 1.0 );

    // Combine results
    vec3 ambient = light.ambient * vec3( texture( material.diffuse, TexCoords ) );
    vec3 diffuse = light.diffuse * diff * vec3( texture( material.diffuse, TexCoords ) );
    vec3 specular = light.specular * spec * vec3( texture( material.specular, TexCoords ) );

    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return ( ambient + diffuse + specular );
}
