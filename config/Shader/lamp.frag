#version 330 core
out vec4 outColor;
  
in vec3 Color;
in vec2 TexCoord;

uniform sampler2D ourTexture;

void main()
{
    vec4 texColor = texture(ourTexture, TexCoord);
    if (texColor.a < 0.1)
        discard;
    outColor = vec4(Color, 1.0) * texColor;
}