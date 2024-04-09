#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include "../Graphics/Texture.h"

class GraphicsSpriteBatch
{
public:
	GraphicsSpriteBatch(const wchar_t* filename, size_t masSprites);
	~GraphicsSpriteBatch();
	void render(float dx, float dy, float dw, float dh);
	void render(float dx, float dy, float dw, float dh, float r, float g, float b, float a, float angle);
	void render(float dx, float dy, float dw, float dh, float r, float g, float b, float a, float angle, float sx, float sy, float sw, float sh);
	void begin();
	void end();

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	D3D11_TEXTURE2D_DESC texture2dDesc;

	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texcoord;
	};

	const size_t maxVertices;
	std::vector<vertex> vertices;
};