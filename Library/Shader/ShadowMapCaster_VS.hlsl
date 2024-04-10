#include "SkinnedMesh.hlsli"

float4 main(VS_IN vin) : SV_POSITION
{   
    float4 blended_position = { 0, 0, 0, 1 };
    for (int bone_index = 0; bone_index < 4; ++bone_index)
    {
        blended_position += vin.boneWeights[bone_index] * mul(vin.position, boneTransforms[vin.boneIndices[bone_index]]);
    }
    vin.position = float4(blended_position.xyz, 1.0f);
    
    return mul(vin.position, mul(world, lightViewProjection));
}