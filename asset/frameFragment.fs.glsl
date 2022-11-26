#version 460

in vec2 texCoords;
out vec4 fragColor;

layout(binding = 0) uniform sampler2D texture0;
layout(binding = 1) uniform sampler2D textureNoise;

uniform int timer;
uniform int testMode;
uniform int filterMode;
uniform bool compareBarEnable;
uniform float compareBarX;
uniform vec2 textureSizeReciprocal;
uniform vec2 frameSize;
uniform vec2 magnifierCenter;
uniform float magnifierRadius;

#define M_PI 3.1415926535897932384626433832795

vec4 medianBlur(vec2 texcoord)
{
    const int blurRangeHalf = 4;
    const int blurRange = 2 * blurRangeHalf + 1;

    vec4 blurColor = vec4(0.0f);
    for (int i = 0; i < blurRange; ++i)
    {
        for (int j = 0; j < blurRange; ++j)
        {
            vec2 temp = texcoord + textureSizeReciprocal * vec2(float(i - blurRangeHalf), float(j - blurRangeHalf));
            blurColor += texture(texture0, temp) / blurRange / blurRange;
        }
    }

    return blurColor;
}

vec4 medianBlur2(vec2 texcoord)
{
    const int blurRangeHalf = 8;
    const int blurRange = 2 * blurRangeHalf + 1;

    vec4 blurColor = vec4(0.0f);
    for (int i = 0; i < blurRange; ++i)
    {
        for (int j = 0; j < blurRange; ++j)
        {
            vec2 temp = texcoord + textureSizeReciprocal * vec2(float(i - blurRangeHalf), float(j - blurRangeHalf));
            blurColor += texture(texture0, temp) / blurRange / blurRange;
        }
    }
    return blurColor;
}

vec4 quantization(vec2 texcoord)
{
    vec4 quantizationColor;
    float nbins = 4.0;
    vec4 color = texture(texture0, texcoord);
    quantizationColor = floor(color * nbins) / nbins;

    return quantizationColor;
}

vec4 differenceOfGaussian(vec2 texcoord)
{

    const float sigma_e = 2.0f;
    const float sigma_r = 2.8f;
    const float phi = 3.4f;
    const float tau = 0.99f;
    const float twoSigmaESquared = 2.0 * sigma_e * sigma_e;
    const float twoSigmaRSquared = 2.0 * sigma_r * sigma_r;
    const int halfWidth = int(ceil( 2.0 * sigma_r ));

    vec4 DOGColor;
    vec2 sum = vec2(0.0);
    vec2 norm = vec2(0.0);
    for (int i = -halfWidth; i <= halfWidth; ++i) {
        for (int j = -halfWidth; j <= halfWidth; ++j) {
            float d = length(vec2(i,j));
            vec2 kernel= vec2(exp(-d * d / twoSigmaESquared),exp(-d * d / twoSigmaRSquared));
            vec4 c= texture(texture0,  texcoord + vec2(i,j) * textureSizeReciprocal);
            vec2 L= vec2(0.299 * c.r + 0.587 * c.g + 0.114 * c.b);
            norm += kernel;
            sum += kernel * L;
        }
    }
    sum /= norm;
    float H = 100.0 * (sum.x - tau * sum.y);
    float edge =( H > 0.0 )?1.0:2.0 * smoothstep(-2.0, 2.0, phi * H );
    DOGColor = vec4(edge, edge, edge, 1.0);

    return DOGColor;
}

vec4 imageAbstraction(vec2 texcoord)
{
    vec4 BQColor = (medianBlur(texcoord) + quantization(texcoord)) / 2.0f;
    vec4 DOGColor = differenceOfGaussian(texcoord);
    return DOGColor * BQColor;
}

vec4 quantize(vec4 color, float n)
{
    color.x = floor(color.x * 255.0f / n) * n / 255.0f;
    color.y = floor(color.y * 255.0f / n) * n / 255.0f;
    color.z = floor(color.z * 255.0f / n) * n / 255.0f;

    return color;
}

vec4 waterColor(vec2 texcoord)
{
    const vec2 texSize = vec2(256.0f, 256.0f);
    vec4 noiseColor = 2 * texture(textureNoise, texcoord);
    vec2 newUV = vec2(texcoord.x + noiseColor.x / texSize.x, texcoord.y + noiseColor.y / texSize.y);
    vec4 fColor = texture(texture0, newUV);                  

    vec4 color1 = quantize(fColor, 255.0f / pow(2.0f, 3));
    vec4 color2 = medianBlur(texcoord);
    return color1 * 0.7 + color2 * 0.3;
}

vec4 magnifier(vec2 texcoord)
{
    const vec2 center = magnifierCenter / frameSize;
    vec2 coord;
    vec4 color;

    if (distance(gl_FragCoord.xy, magnifierCenter - vec2(0.0f, magnifierRadius + 1)) < 6)
    {
        color = vec4(0.5f);
    }
    else if (distance(gl_FragCoord.xy, magnifierCenter - vec2(0.0f, magnifierRadius + 1)) < 8)
    {
        color = vec4(1.0f);
    }
    else if (distance(gl_FragCoord.xy, magnifierCenter) < magnifierRadius)
    {
        coord = center + (texcoord - center) / 2.0f;
        color = texture(texture0, coord);
    }
    else if (distance(gl_FragCoord.xy, magnifierCenter) < magnifierRadius + 2)
    {
        color = vec4(1.0f);
    }
    else
    {
        color = texture(texture0, texcoord);
    }
    return color;
}

vec4 bloomEffect(vec2 texcoord)
{
    vec4 color1 = texture(texture0, texcoord);
    vec4 color2 = medianBlur(texcoord);
    vec4 color3 = medianBlur2(texcoord);
    return color1 * 0.7 + color2 * 0.3  + color3  * 0.2;
}

vec4 pixelization(vec2 texcoord)
{
    const float pixels = 512.0;
    const float dx = 8.0 * (1.0 / pixels);
    const float dy = 8.0 * (1.0 / pixels);
    vec2 coord = vec2(dx * floor(texcoord.x / dx), dy * floor(texcoord.y / dy));
    vec4 color = texture(texture0, coord);
    return color;
}

vec4 sineWave(vec2 texcoord)
{
    float offset = 10;
    vec2 coord = texcoord;
    coord.x += 0.06 * sin(radians((texcoord.y * 500) + timer * 2));
    vec4 color = texture(texture0, coord);
    return color;
}

void filterDraw()
{
    if (filterMode == 1)
        fragColor = imageAbstraction(texCoords);
    else if (filterMode == 2)
        fragColor = waterColor(texCoords);
    else if (filterMode == 3)
        fragColor = magnifier(texCoords);
    else if (filterMode == 4)
        fragColor = bloomEffect(texCoords);
    else if (filterMode == 5)
        fragColor = pixelization(texCoords);
    else if (filterMode == 6)
        fragColor = sineWave(texCoords);
    else 
        fragColor = texture(texture0, texCoords);
}

void compareBar()
{
    float diifferentX = gl_FragCoord.x - compareBarX;
    if (abs(diifferentX) < 4 && distance(gl_FragCoord.y, frameSize.y / 2) < 18)
    {
        fragColor = vec4(0.5f);
    }
    else if (abs(diifferentX) <= 6 && distance(gl_FragCoord.y, frameSize.y / 2) <= 20)
    {
        fragColor = vec4(1.0f);
    }
    else if (abs(diifferentX) <= 1)
    {
        fragColor = vec4(1.0f);
    }
    else if (diifferentX < -1)
    {
        filterDraw();
    }
    else
        fragColor = texture(texture0, texCoords);
}

void main()
{
    if (filterMode == 3 || filterMode == 0 || !compareBarEnable)
        filterDraw();
    else
        compareBar();
}