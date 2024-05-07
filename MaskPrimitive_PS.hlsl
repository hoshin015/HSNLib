#include "MaskPrimitive.hlsli"

float4 main(VS_IN vin) : SV_TARGET
{
    float2 p = vin.texcoord;
    float2 a = circlePos;
    float d = length(p - a);
    float4 color = vin.color * (d>radius);
    return float4(color);
}