#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "../Graphics/Texture.h"

class Sprite3D
{
public:
	Sprite3D(const wchar_t* filename);
	~Sprite3D();
	void Render(const DirectX::XMFLOAT4X4& world, DirectX::XMFLOAT4 color);
	void Render(const DirectX::XMFLOAT4X4& world, DirectX::XMFLOAT4 color, float sx, float sy, float sw, float sh);

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	D3D11_TEXTURE2D_DESC texture2dDesc;

	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texcoord;
	};

	struct Constants
	{
		DirectX::XMFLOAT4X4 world;
	};
};