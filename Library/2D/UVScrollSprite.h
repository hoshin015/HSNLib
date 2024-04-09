#pragma once
#include "Sprite.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include "../Graphics/Texture.h"

class UVScrollSprite
{
public:
	UVScrollSprite(const wchar_t* filename);
	~UVScrollSprite();
	void Render(DirectX::XMFLOAT2 uvScrollValue, float dx, float dy, float dw, float dh, float r, float g, float b, float a, float angle);
	void Render(DirectX::XMFLOAT2 uvScrollValue, float dx, float dy, float dw, float dh, float r, float g, float b, float a, float angle, float sx, float sy, float sw, float sh);

protected:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	D3D11_TEXTURE2D_DESC texture2dDesc;

	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texcoord;
	};

	struct Constants
	{
		DirectX::XMFLOAT2 uvScrollValue;
		DirectX::XMFLOAT2 pad;
	};

};