#pragma once
#include "MeshEffect.h"

class DomeEffect : public MeshEffect
{
public:
	DomeEffect();
	~DomeEffect() {}

	// 更新処理
	void Update() override;

	// 描画処理
	void Render() override;

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;

	struct Constants
	{
		DirectX::XMFLOAT2 uvScrollValue;
		DirectX::XMFLOAT2 pad;
	};

	DirectX::XMFLOAT2 uvScrollValue = { 0,0 };

	float nowScale = 5.0f;			// スケール
	float nowAlpha = 1.0f;			// アルファ
	float uvScrollSpeed = 0.2f;		// UVスクロールのスピード
};