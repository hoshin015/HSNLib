#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <DirectXMath.h>
#include <mutex>
#include "FrameBuffer.h"
#include "BloomBuffer.h"
#include "ShadowBuffer.h"
#include "FullScreenQuad.h"

enum class SAMPLER_STATE
{
	POINT,
	LINEAR,
	ANISOTROPIC,
	TEXT_LINEAR,
	LINEAR_BORDER_BLACK,
	LINEAR_BORDER_WHITE,
	SHADOWMAP,

	NUM,
};

enum class RASTERIZER_STATE
{
	CLOCK_FALSE_SOLID,
	CLOCK_FALSE_WIREFRAME,
	CLOCK_FALSE_CULL_NONE,
	CLOCK_FALSE_WIREFRAME_CULL_NONE,
	CLOCK_TRUE_SOLID,
	CLOCK_TRUE_WIREFRAME,
	CLOCK_TRUE_CULL_NONE,
	CLOCK_TRUE_WIREFRAME_CULL_NONE,

	NUM,
};

enum class BLEND_STATE
{
	NONE,
	ALPHA,
	ADD,
	MULTIPLY,

	NUM,
};

enum class DEPTHSTENCIL_STATE
{
	ZT_ON_ZW_ON,
	ZT_ON_ZW_OFF,
	ZT_OFF_ZW_ON,
	ZT_OFF_ZW_OFF,

	NUM,
};

enum class OBJECT_TYPE
{
	OFF_LUMINANCE,
	ON_LUMINANCE,
};

enum class PS_TYPE
{
	LuminanceExtraction_PS,
	Blur_PS,
	ColorFilter_PS,
	GaussianBlur_PS,
	BloomFinalPass_PS,

	SkinnedMesh_PS,

	NUM
};

enum class VS_TYPE
{
	GaussianBlurX_VS,
	GaussianBlurY_VS,
	ShadowMapCaster_VS,

	SkinnedMesh_VS,

	NUM
};



//--------------------------------------------------------------
//  Graphics
//--------------------------------------------------------------
class Graphics
{
private:
	Graphics() {}
	~Graphics() {}

public:
	static Graphics& Instance()
	{
		static Graphics instance;
		return instance;
	}


	IDXGIDevice1* pDXGI = NULL;
	IDXGIAdapter* pAdapter = NULL;
	IDXGIFactory* pFactory = NULL;
	void CreateSwapchain(HWND hwnd, int windowWidth, int windowHeight);

	// ミューテックス取得
	std::mutex& GetMutex() { return mutex; }

	// 初期化
	void Initialize(HWND hwnd, int windowWidth, int windowHeight);

	// 描画開始
	void Begin();
	// 描画終了
	void End();

	// depthStencilの設定
	void SetDepthStencil(DEPTHSTENCIL_STATE state);
	// rasterizerの設定
	void SetRasterizer(RASTERIZER_STATE state);
	// blendの設定
	void SetBlend(BLEND_STATE state);

public:
	bool sync = false;

	// 平行光源情報
	struct DirectionalLightData
	{
		DirectX::XMFLOAT4 direction;	// 向き
		DirectX::XMFLOAT4 color;		// 色
	};
	// 点光源情報
	struct PointLightData
	{
		DirectX::XMFLOAT4 position;		// 座標
		DirectX::XMFLOAT4 color;		// 色
		float range;					// 範囲
		DirectX::XMFLOAT3 pad;
	};
	// 点光源の最大数
	static constexpr int PointLightMax = 8;
	// スポットライト情報
	struct SpotLightData
	{
		DirectX::XMFLOAT4 position;		// 座標
		DirectX::XMFLOAT4 direction;	// 向き
		DirectX::XMFLOAT4 color;		// 色
		float range;					// 範囲
		float innerCorn;				// インナー角度範囲
		float outerCorn;				// アウター角度範囲
		float pad;
	};
	// スポットライトの最大数
	static constexpr int SpotLightMax = 8;

	// ガウスフィルター計算情報
	//struct GaussianFilterData
	//{
	//	int kernelSize = 8;				// カーネルサイズ
	//	float deviation = 10.0f;		// 標準偏差
	//	DirectX::XMFLOAT2 textureSize;	// 暈すテクスチャのサイズ
	//};
	//static const int MaxKernelSize = 16;

	// カラーフィルター情報
	struct ColorFilter
	{
		float hueShift;
		float saturation;
		float brightness;
		float pad;
	};
	ColorFilter colorFilterConstant;

	static const int NUM_WEIGHTS = 8;
	struct GaussianConstant
	{
		float weights[NUM_WEIGHTS];
	};
	GaussianConstant gaussianConstant;

	struct LuminanceExtractionConstant
	{
		float threshould;
		float intensity;
		DirectX::XMFLOAT2 lumiPad;
	};
	LuminanceExtractionConstant luminanceExtractionConstant;

	struct ShadowMapData
	{
		DirectX::XMFLOAT4X4 lightViewProjection;				// ライトビュープロジェクション行列
		DirectX::XMFLOAT3 shadowColor = { 0.2f,0.2f,0.2f };		// 影の色
		float shadowBias = 0.0001f;								// 深度比較用のオフセット値
	};
	ID3D11ShaderResourceView* shadowMap;						// シャドウマップテクスチャ
	float shadowDrawRect = 200.0f;
	ShadowMapData shadowMapData;

	// 定数バッファ用構造体
	struct SceneConstants
	{
		DirectX::XMFLOAT4X4 viewProjection;
		DirectionalLightData directionalLightData;
		PointLightData pointLightData[PointLightMax];
		SpotLightData spotLightData[SpotLightMax];
		int pointLightCount = 0;
		int spotLightCount = 0;
		DirectX::XMFLOAT2 pad;
		DirectX::XMFLOAT4 ambientLightColor;
		DirectX::XMFLOAT4 cameraPosition;
		//GaussianFilterData gaussianFilterData;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapchain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilStates[static_cast<size_t>(DEPTHSTENCIL_STATE::NUM)];
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerStates[static_cast<size_t>(RASTERIZER_STATE::NUM)];
	Microsoft::WRL::ComPtr<ID3D11BlendState> blendStates[static_cast<size_t>(BLEND_STATE::NUM)];
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerStates[static_cast<size_t>(SAMPLER_STATE::NUM)];

	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffers[8];
	std::unique_ptr<FrameBuffer> frameBuffers[8];
	std::unique_ptr<BloomBuffer> bloomBuffer;
	std::unique_ptr<ShadowBuffer> shadowBuffer;
	std::unique_ptr<FullScreenQuad> bitBlockTransfer;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShaders[8];
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShaders[8];
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayouts[8];
public:

	// ガウシアン関数を利用して重みテーブルを計算する
	void CalcWeightsTableFromGaussian(float blurPower);

private:
	std::mutex mutex;
};