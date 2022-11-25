#version 460

layout(location = 0) out vec4 fragColor;

uniform mat4 um4mv;
uniform mat4 um4p;

in VertexData
{
    vec3 N; // eye space normal
    vec3 L; // eye space light vector
    vec3 H; // eye space halfway vector
    vec3 normal;
    vec2 texcoord;
} vertexData;


uniform int outputMode;
// uniform sampler2D tex;
uniform sampler2D textureDiffuse1;
uniform sampler2D textureDiffuse2;

void main()
{
    // fragColor = texture(textureDiffuse1 ,vertexData.texcoord);

    if (outputMode == 0)
        fragColor = texture(textureDiffuse1 ,vertexData.texcoord);
    else
        fragColor = vec4(vertexData.normal, 1.0);
}
