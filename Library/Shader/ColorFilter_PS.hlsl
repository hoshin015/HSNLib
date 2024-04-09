#include "FullScreenQuad.hlsli"
#include "FilterFunctions.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2

SamplerState samplerStates[3] : register(s0);
Texture2D textureMaps[4] : register(t0);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = textureMaps[0].Sample(samplerStates[LINEAR], pin.texcoord);
    
    // RGB > HSV ‚É•ÏŠ·
    color.rgb = RGB2HSV(color.rgb);
    
    // F‘Š’²®
    color.r += hueShift;
    
    // Ê“x’²®
    color.g *= saturation;
    
    // –¾“x’²®
    color.b *= brightness;
    
    // HSV > RGB ‚É•ÏŠ·
    color.rgb = HSV2RGB(color.rgb);
    
    return color;
}