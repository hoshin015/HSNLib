#include "SkinnedMesh.hlsli"


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
    
#if 0
    // Inverse gamma process
    const float GAMMA = 2.2;
    diffuseColor.rgb = pow(diffuseColor.rgb, GAMMA);
#endif
    
    
    // 法線マップの取得
    float4 normal = texture_maps[1].Sample(sampler_states[LINEAR], pin.texcoord);
    normal = (normal * 2.0) - 1.0;
    float3 n = normalize(pin.worldNormal.xyz);
    float3 t = normalize(pin.worldTangent.xyz);
    float sigma = pin.worldTangent.w;
    float3 b = normalize(cross(n, t) * sigma);
    float3 N = normalize((normal.x * t) + (normal.y * b) + (normal.z * n));
    
    // スペキュラマップの取得
    float specularPower = texture_maps[2].Sample(sampler_states[LINEAR], pin.texcoord).r;
    
    // AOマップの取得
    float ambientPower = texture_maps[3].Sample(sampler_states[LINEAR], pin.texcoord);
    
    // 表面材質の性質
    float3 Kd = float3(0.8, 0.8, 0.8);
    float3 Ks = float3(1, 1, 1);
    float3 Ka = float3(1, 1, 1);
    
    // --- ディレクションライト ---
    float3 L = normalize(directionalLightData.direction.xyz);
    
    // 拡散反射光
    float3 directionDiffuse = CalcLambertDiffse(N, L, directionalLightData.color.rgb, Kd);
    
    // 鏡面反射光
    float3 R = reflect(L, N); // 反射ベクトルを求めている
    float3 ToCamera = normalize(cameraPosition.xyz - pin.worldPosition.xyz); // 光が入射したサーフェースから視点に向かって伸びるベクトルを求めている
    float3 directionSpecular = CalcPhongSpecular(N, L, directionalLightData.color.rgb, ToCamera, 128.0f, Ks);
    
    // 平行光源の影なので、平行光源に対して影を適応
    float3 shadow = CalcShadowColorPCFFilter(shadowMap, shadowSampler, pin.shadowTexcoord, shadowColor, shadowBias);
    directionDiffuse *= shadow;
    directionSpecular *= shadow;

    // --- ポイントライト ---
    float3 pointDiffuse = (float3) 0;
    float3 pointSpecular = (float3) 0;
    int i;
    for (i = 0; i < pointLightCount; ++i)
    {
        // ライトのベクトルを算出
        float3 lightVector = pin.worldPosition.xyz - pointLightData[i].position.xyz;
        float lightLength = length(lightVector);
        
        // ライトの範囲外なら計算しない
        if (lightLength >= pointLightData[i].range)
            continue;
        
        // 影響力は距離に比例して小さくなっていく
        float attenuate = saturate(1.0f - lightLength / pointLightData[i].range);
        
        lightVector = lightVector / lightLength;    // 正規化
        pointDiffuse += CalcLambertDiffse(N, lightVector, pointLightData[i].color.rgb, Kd) * attenuate;
        pointSpecular += CalcPhongSpecular(N, lightVector, pointLightData[i].color.rgb, ToCamera, 128.0f, Ks) * attenuate;
    }
    
    // --- スポットライト ---
    float3 spotDiffuse = (float3) 0;
    float3 spotSpecular = (float3) 0;
    for (i = 0; i < spotLightCount; ++i)
    {
        // ライトベクトルを算出
        float3 lightVector = pin.worldPosition.xyz - spotLightData[i].position.xyz;
        float lightLength = length(lightVector);
        
        // ライトの範囲外なら計算しない
        if (lightLength >= spotLightData[i].range)
            continue;
        
        // 影響力は距離に比例して小さくなっていく
        float attenuate = saturate(1.0f - lightLength / spotLightData[i].range);
        
        lightVector = lightVector / lightLength; // 正規化
        
        // 角度減数を算出してattenuateに乗算する
        float3 spotDirection = normalize(spotLightData[i].direction.rgb);
        float angle = dot(spotDirection, lightVector);
        float area = spotLightData[i].innerCorn - spotLightData[i].outerCorn;
        attenuate *= saturate(1.0f - (spotLightData[i].innerCorn - angle) / area);
        
        spotDiffuse += CalcLambertDiffse(N, lightVector, spotLightData[i].color.rgb, Kd) * attenuate;
        spotSpecular += CalcPhongSpecular(N, lightVector, spotLightData[i].color.rgb, ToCamera, 128.0f, Ks) * attenuate;
    }
    
    // 環境光
    float3 ambient = ambientLightColor.rgb * Ka * ambientPower;
    
    // 拡散反射光と鏡面反射光から最終的な光を計算    
    float3 diffuseLig = directionDiffuse + pointDiffuse + spotDiffuse;
    float3 specularLig = (directionSpecular + pointSpecular + spotSpecular) * specularPower;

    float3 lig = diffuseLig + specularLig + ambient;
    
    // 最終的なカラーを計算
    float4 finalColor = float4(lig * diffuseColor.rgb, diffuseColor.a);
    
#if 1
    PSOUT output = (PSOUT)0;
    output.color = finalColor;
    
    output.flag = float4(0,0,0,1);
    output.flag.r = objectType;
    
    return output;
#else
    return finalColor;
#endif
}
