#pragma once
#include "MeshEffect.h"

class ParryEffect : public MeshEffect
{
public:
	ParryEffect(float scale);
	~ParryEffect() {}

	// �X�V����
	void Update() override;

	// �`�揈��
	void Render() override;

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

	float nowAlpha = 0.3f;			// �A���t�@
	float uvScrollSpeed = 0.2f;		// UV�X�N���[���̃X�s�[�h
};