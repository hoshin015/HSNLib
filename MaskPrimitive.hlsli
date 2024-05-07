struct VS_IN {
    float3 position : POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

struct VS_OUT {
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

cbuffer Cbuffer : register(b7) {
    float2 circlePos;
    float radius;
}