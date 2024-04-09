#include "FullScreenQuad.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2
#define TEXT_LINEAR 3
#define LINEAR_BORDER_BLACK 4
#define LINEAR_BORDER_WHITE 5

SamplerState sampler_states[5] : register(s0);
Texture2D texture_map : register(t0);

float4 main(VS_OUT pin) : SV_TARGET
{
    return texture_map.Sample(sampler_states[LINEAR_BORDER_BLACK], pin.texcoord);
}