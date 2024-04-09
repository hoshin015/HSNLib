#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "../Graphics/Texture.h"

class Primitive2D
{
public:
	Primitive2D();
	~Primitive2D();

	void Render(float dx, float dy, float dw, float dh, float r, float g, float b, float a, float angle);

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;

	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
	};
};