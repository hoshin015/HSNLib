#include "Mask.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2

SamplerState samplerStates[3] : register(s0);
Texture2D texture0 : register(t0);

Texture2D maskTexture : register(t1);

float4 main(VS_OUT pin) : SV_TARGET
{
    float4 color = texture0.Sample(samplerStates[LINEAR], pin.texcoord) * pin.color;
    
    float mask = maskTexture.Sample(samplerStates[LINEAR], pin.texcoord).r;
    
    // step関数を用いてmaskの値とdissolveThresholdの値を比較して透過値を0 or 1にする
    float alpha = step(mask, dissolveThreshold);
    
    // 縁の処理
    float edgeValue = step(mask - dissolveThreshold, dissolveThreshold) * step(dissolveThreshold, mask) * step(mask, dissolveThreshold + edgeThreshold);

    color.rgb += edgeColor.rgb * edgeValue;
    alpha = saturate(alpha + edgeValue);

	// colorの透過値に乗算する
    color.a *= alpha;

	// アルファが0以下ならそもそも描画しないようにする
    clip(color.a - 0.01f);

    return color;
}