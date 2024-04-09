#include "SkinnedMeshPBR.hlsli"


#define POINT 0
#define LINEAR 1
#define ANISOTROPIC 2

SamplerState sampler_states[3] : register(s0);
Texture2D texture_maps[4] : register(t0);
float4 main(VS_OUT pin) : SV_TARGET
{   
    // アルベドカラー (拡散反射光)
    float4 albedoColor = texture_maps[0].Sample(sampler_states[ANISOTROPIC], pin.texcoord);
    
    // 法線マップの取得
    float4 normal = texture_maps[1].Sample(sampler_states[LINEAR], pin.texcoord);
    normal = (normal * 2.0) - 1.0;
    float3 n = normalize(pin.worldNormal.xyz);
    float3 t = normalize(pin.worldTangent.xyz);
    float sigma = pin.worldTangent.w;
    float3 b = normalize(cross(n, t) * sigma);
    float3 N = normalize((normal.x * t) + (normal.y * b) + (normal.z * n));
    
    // スペキュラマップカラー
    float3 specColor = albedoColor;
    
    // 金属度
    float metallic = texture_maps[2].Sample(sampler_states[LINEAR], pin.texcoord).r;
    
    // 滑らかさ
    float smooth = texture_maps[2].Sample(sampler_states[LINEAR], pin.texcoord).a;
    
    // AOマップの取得
    float ambientPower = texture_maps[3].Sample(sampler_states[LINEAR], pin.texcoord);
    
    // 表面材質の性質
    float3 Kd = float3(0.8, 0.8, 0.8);
    float3 Ks = float3(1, 1, 1);
    float3 Ka = float3(1, 1, 1);
    
    float3 ToCamera = normalize(cameraPosition.xyz - pin.worldPosition.xyz); // 光が入射したサーフェースから視点に向かって伸びるベクトルを求めている
    // --- ディレクションライト ---
    float3 L = normalize(directionalLightData.direction.xyz);
    
    // フレネル反射を考慮した拡散反射を計算
    float diffuseFromFrenel = CalcDiffuseFromFresnel(N, L, ToCamera);
    // 正規化 Lambert 拡散反射を求める
    float3 lambertDiffuse = CalcLambertDiffse(N, L, directionalLightData.color.rgb, Kd) / PI;
    // 最終的な拡散反射光を求める
    float3 diffuse = albedoColor * diffuseFromFrenel * lambertDiffuse;
    
    
    
    // 鏡面反射光
    float3 R = reflect(L, N); // 反射ベクトルを求めている
    float3 directionSpecular = CalcPhongSpecular(N, L, directionalLightData.color.rgb, ToCamera, 128.0f, Ks);
    float3 spec = CookTorranceSpecular(L, ToCamera, N, smooth) * directionalLightData.color.rgb;
    
    spec *= lerp(float3(1.0f, 1.0f, 1.0f), specColor, metallic);
    
    // 環境光
    float3 ambient = albedoColor.rgb * Ka;
    
    float3 lig = diffuse * (1.0f - smooth) + spec + ambient * ambientPower;
    
    // 最終的なカラーを計算
    float4 finalColor = float4(lig, albedoColor.a);
    
    return finalColor;
}
