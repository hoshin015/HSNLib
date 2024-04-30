#pragma once
#include "MeshEffect.h"

class DomeEffect : public MeshEffect
{
public:
	DomeEffect(float radius);
	~DomeEffect() {}

	// �X�V����
	void Update() override;

	// �`�揈��
	void Render() override;

	// ���݂̃A���t�@�l�Q�b�^�[
	float GetNowAlpha() { return nowAlpha; }

	// �G�t�F�N�g�J�n
	void StartEffect();

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

	float nowScale = 5.0f;			// �X�P�[��
	float maxScale = 5.0f;			// �ő�X�P�[��
	float nowAlpha = 3.0f;			// �A���t�@
	float uvScrollSpeed = 2.2f;		// UV�X�N���[���̃X�s�[�h
};