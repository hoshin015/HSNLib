#pragma once
#include "MeshEffect.h"

class ParryEffect : public MeshEffect
{
public:
	ParryEffect();
	~ParryEffect() {}

	// XVˆ—
	void Update() override;

	// •`‰æˆ—
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
	float nowScale = 5.0f;
	float nowAlpha = 1.0f;
};