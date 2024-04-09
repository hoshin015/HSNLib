#include "GeometricPrimitive.hlsli"

VS_OUT main(float4 poisition : POSITION)
{
    VS_OUT vout;
    // ローカル座標からワールド座標に変換しその後、スクリーン座標に変換
    vout.position = mul(poisition, mul(world, viewProjection));
    vout.color = materialColor;
    return vout;
}