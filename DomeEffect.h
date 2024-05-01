#pragma once
#include "MeshEffect.h"

class DomeEffect : public MeshEffect
{
public:
	DomeEffect(float radius);
	~DomeEffect() {}

	// 更新処理
	void Update() override;

	// 描画処理
	void Render() override;

	// 現在のアルファ値ゲッター
	float GetNowAlpha() { return nowAlpha; }

	// エフェクト開始
	void StartEffect(float maxScale, DirectX::XMFLOAT3 color = { 1,0.62,1 });

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> storeVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> storePixelShader;

	struct Constants
	{
		DirectX::XMFLOAT2 uvScrollValue;
		DirectX::XMFLOAT2 pad;
	};

	DirectX::XMFLOAT2 uvScrollValue = { 0,0 };
	DirectX::XMFLOAT3 color = { 1,0.62,1 };

	float nowScale = 5.0f;			// スケール
	float maxScale = 5.0f;			// 最大スケール
	float nowAlpha = 0.0f;			// アルファ
	float uvScrollSpeed = 2.2f;		// UVスクロールのスピード
};