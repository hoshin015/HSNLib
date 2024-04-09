struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

cbuffer CbScene : register(b0)
{
    float2 uvScrollValue;
    float2 pad;
}