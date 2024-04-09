// 平行光源情報
struct DirectionalLightData
{
    float4 direction;   // 向き
    float4 color;       // 色
};
// 点光源情報
struct PointLightData
{
    float4 position;
    float4 color;
    float range;
    float3 pad;
};
// 点光源の最大数
static const int PointLightMax = 8;
// スポットライト
struct SpotLightData
{
    float4 position;
    float4 direction;
    float4 color;
    float range;
    float innerCorn;
    float outerCorn;
    float dummy;
};
// スポットライトの最大数
static const int SpotLightMax = 8;


//--------------------------------------------
//	ランバート拡散反射計算関数
//--------------------------------------------
// normal		: 法線(正規化済み)
// lightVector	: 入射ベクトル(正規化済み)
// lightColor	: 入射光色
// kd			: 反射率(反射の強さ)
// 返す値		: 拡散反射色
float3 CalcLambertDiffse(float3 normal, float3 lightVector, float3 lightColor, float3 kd)
{ 
    float3 diffuse = max(0, dot(normal, -lightVector)); // -を付けているのは内積の結果がベクトルが向き合っている場合を 1 にするため
    return kd * lightColor * diffuse;
}

//--------------------------------------------
//	フォンの鏡面反射計算関数
//--------------------------------------------
// normal		: 法線(正規化済み)
// lightVector	: 入射ベクトル(正規化済み)
// lightColor	: 入射光色
// eyeVector	: 視線ベクトル(正規化済み)
// shininess	: 光沢度(質感調整値)
// ks			: 反射率(反射の強さ)
// 返す値		: 鏡面反射色
float3 CalcPhongSpecular(float3 normal, float3 lightVector, float3 lightColor, float3 eyeVector, float shininess, float3 ks)
{
    float3 R = reflect(lightVector, normal); // 反射ベクトルを求めている
    float3 specular = pow(max(0, dot(R, eyeVector)), shininess);
    return ks * lightColor * specular;
}