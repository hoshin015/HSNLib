struct VS_OUT
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD;
};

cbuffer COLOR_FILTER : register(b3)
{
    float hueShift;
    float saturation;
    float brightness;
    float pad;
}