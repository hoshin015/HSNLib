#include "FullScreenQuad.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2

SamplerState samplerStates[3] : register(s0);
Texture2D textureMap : register(t0);
Texture2D bloomMap : register(t1);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = textureMap.Sample(samplerStates[POINT], pin.texcoord);
    
    // ブルームテクスチャを加算する
    color.rgb += bloomMap.Sample(samplerStates[POINT], pin.texcoord).rgb;
    
    return color;

}