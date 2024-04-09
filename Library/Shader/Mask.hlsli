struct VS_OUT
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texcoord : TEXCOORD;
};

//	スプライト描画単位で指定するパラメーター
cbuffer CbMask : register(b0) // 0番にある定数バッファをこれで利用する
{
    float dissolveThreshold;    // ディゾルブ量
    float edgeThreshold;        // 緑の閾値
    float2 pad;
    float4 edgeColor;           // 緑の色
};
