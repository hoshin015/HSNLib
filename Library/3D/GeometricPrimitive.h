#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>

class GeometricPrimitive
{
private:
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
	};
	struct Constants
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4	materialColor;
	};

protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	UINT	vertexCount = 0;


public:
	// �R���X�g���N�^
	GeometricPrimitive();
	// �p����̃N���X�Ńf�X�g���N�^���ĂԂ���
	virtual ~GeometricPrimitive() = default;

	// �`��
	void Render(const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& materialColor);

private:
	// ���b�V�������@(�p����̃N���X�ō쐬����
	virtual void CreateMesh() = 0;
};