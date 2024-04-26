#include "ParryEffect.hlsli"

VS_OUT main(VS_IN vin)
{
    vin.normal.w = 0;

    float sigma = vin.tangent.w;
    vin.tangent.w = 0;
    
    float4 blended_position = { 0, 0, 0, 1 };
    float4 blended_normal = { 0, 0, 0, 0 };
    float4 blended_tangent = { 0, 0, 0, 0 };
    for (int bone_index = 0; bone_index < 4; ++bone_index)
    {
        blended_position += vin.boneWeights[bone_index] * mul(vin.position, boneTransforms[vin.boneIndices[bone_index]]);
        blended_normal += vin.boneWeights[bone_index] * mul(vin.normal, boneTransforms[vin.boneIndices[bone_index]]);
        blended_tangent += vin.boneWeights[bone_index] * mul(vin.tangent, boneTransforms[vin.boneIndices[bone_index]]);
    }
    vin.position = float4(blended_position.xyz, 1.0f);
    vin.normal = float4(blended_normal.xyz, 0.0f);
    vin.tangent = float4(blended_tangent.xyz, 0.0f);
    
    VS_OUT vout;
    vout.position = mul(vin.position, mul(world, viewProjection));
    
    vout.worldPosition = mul(vin.position, world);
    vout.worldNormal = normalize(mul(vin.normal, world));
    vout.worldTangent = normalize(mul(vin.tangent, world));
    vout.worldTangent.w = sigma;
    
    vout.texcoord = vin.texcoord + uvScrollValue;
    vout.color = materialColor;
    
    vout.shadowTexcoord = CalcShadowTexcoord(blended_position.xyz, mul(world, lightViewProjection));
     
    return vout;
}
