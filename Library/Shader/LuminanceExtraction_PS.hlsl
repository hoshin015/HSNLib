#include "FullScreenQuad.hlsli"
#include "FilterFunctions.hlsli"

cbuffer LuminanceExtractionConstant : register(b0)
{
    float threshould;   // 高輝度抽出のための閾値
    float intensity;    // ブルームの強度
    float2 lumiPad;
}

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2

SamplerState samplerStates[3] : register(s0);
Texture2D textureMaps    : register(t0);
Texture2D bloomFilterMap : register(t1);

float4 main(VS_OUT pin) : SV_TARGET
{
    // 普通に描画されたテクスチャ
    float4 color = textureMaps.Sample(samplerStates[POINT], pin.texcoord);
    
    // r が 1 なら次のテクスチャに色を書きこむ
    float4 flag = bloomFilterMap.Sample(samplerStates[POINT], pin.texcoord);
    
    // RGB > 輝度値に変換
    float luminance = RGB2Luminance(color.rgb);
    clip(luminance - 0.01f);
    
    // 閾値との差を算出
    float contribution = max(0, luminance - threshould);
    
    // 出力する色を補正する
    contribution /= luminance;
    color.rgb *= contribution * intensity;
    
    //return (flag.r != 0) ? color : 0;
    return flag.r * color;
    //return flag;
}