#include "Sprite3D.hlsli"

Texture2D colorMap : register(t0);
SamplerState pointSamplerState : register(s0);
SamplerState linearSamplerState : register(s1);
SamplerState anisotropicSamplerState : register(s2);

float4 main(VS_OUT pin) : SV_TARGET
{
    return colorMap.Sample(linearSamplerState, pin.texcoord) * pin.color;
    //return pin.color;
}