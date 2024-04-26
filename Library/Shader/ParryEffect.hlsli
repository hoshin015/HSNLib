#include "Light.hlsli"
#include "ShadowmapFunctions.hlsli"

struct VS_IN
{
    float4 position : POSITION;
    float4 normal : NORMAL;
    float4 tangent : TANGENT;
    float2 texcoord : TEXCOORD;
    float4 boneWeights : WEIGHTS;
    uint4 boneIndices : BONES;
};

struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 worldPosition : POSITION;
    float4 worldNormal : NORMAL;
    float4 worldTangent : TANGENT;
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;
    float3 shadowTexcoord : TEXCOORD1;
};

static const int MAX_BONES = 256;
cbuffer OBJECT_CONSTANT_BUFFER : register(b0)
{
    row_major float4x4 world;
    float4 materialColor;
    row_major float4x4 boneTransforms[MAX_BONES];
}

cbuffer SCENE_CONSTANT_BUFFER : register(b1)
{
    row_major float4x4 viewProjection;
    DirectionalLightData directionalLightData;
    PointLightData pointLightData[PointLightMax];
    SpotLightData spotLightData[SpotLightMax];
    int pointLightCount;
    int spotLightCount;
    float2 pad1;
    float4 ambientLightColor;
    float4 cameraPosition;
}

cbuffer IS_BLOOM : register(b2)
{
    float objectType;
    float3 pad2;
}

cbuffer CbShadowmap : register(b3)
{
    row_major float4x4 lightViewProjection; // ライトビュープロジェクション行列
    float3 shadowColor; // 影の色
    float shadowBias; // 深度値比較時のオフセット
};

cbuffer CbSUvScroll : register(b7)
{
    float2 uvScrollValue;
    float2 pad;
};