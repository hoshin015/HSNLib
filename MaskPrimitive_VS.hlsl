#include "MaskPrimitive.hlsli"
VS_OUT main(VS_IN vin)
{
    VS_OUT vout;
    vout.position = float4(vin.position, 1);
    vout.color = vin.color;
    vout.texcoord = vin.texcoord;
	return vout;
}