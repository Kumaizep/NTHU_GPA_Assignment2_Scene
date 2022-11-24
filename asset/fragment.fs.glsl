#version 410

layout(location = 0) out vec4 fragColor;

uniform mat4 um4mv;
uniform mat4 um4p;

in VertexData
{
    vec3 N; // eye space normal
    vec3 L; // eye space light vector
    vec3 H; // eye space halfway vector
    vec2 texcoord;
} vertexData;

// uniform sampler2D tex;
uniform sampler2D textureDiffuse1;
uniform sampler2D textureDiffuse2;

void main()
{
    // vec3 texColor = texture(tex ,vertexData.texcoord).rgb;
    // fragColor = vec4(texColor, 1.0);
    fragColor = texture(textureDiffuse1 ,vertexData.texcoord);
}
