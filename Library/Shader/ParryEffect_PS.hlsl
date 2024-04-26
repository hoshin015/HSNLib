#include "ParryEffect.hlsli"

#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2

SamplerState sampler_states[3] : register(s0);
SamplerState shadowSampler : register(s3);
Texture2D texture_maps[4] : register(t0);
Texture2D shadowMap : register(t4);

#if 1
struct PSOUT
{
    float4 color : SV_TARGET0;
    float4 flag : SV_TARGET1;
};
PSOUT main(VS_OUT pin)
#else
float4 main(VS_OUT pin) : SV_TARGET
#endif
{
    // ディフューズマップの取得
    float4 diffuseColor = texture_maps[0].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
     // エミッシブカラーの取得
    float4 emissive = texture_maps[3].Sample(sampler_states[POINT], pin.texcoord);
    
    // 最終的なカラーを計算
    float4 finalColor;
    
    //finalColor.a = diffuseColor.a;
    
    //finalColor.rgb = diffuseColor.rgb * pin.color.rgb;
    finalColor.rgb = pin.color.rgb;
    finalColor.a = length(diffuseColor.rgb) * pin.color.a;
    
    //if (length(finalColor.rgb) < 0.5)
    //    clip(-1);
   
#if 1
    PSOUT output = (PSOUT) 0;
    output.color = finalColor;
    
    output.flag = float4(0, 0, 0, 1);
    //output.flag.r = (finalColor.a != 0) ? 1 : 0;
    output.flag.r = finalColor.a;
    
    return output;
#else
    return finalColor;
#endif
}
