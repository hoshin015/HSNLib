#include "Sprite3D.hlsli"

VS_OUT main(float4 poisition : POSITION, float4 color : COLOR, float2 texcoord : TEXCOORD)
{
    VS_OUT vout;
    // ローカル座標からワールド座標に変換しその後、スクリーン座標に変換
    vout.position = mul(poisition, mul(world, viewProjection));
    vout.color = color;
    vout.texcoord = texcoord;
    return vout;
}