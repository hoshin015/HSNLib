#pragma once
#include <DirectXMath.h>
#include "FontTexture.h"
#include "../2D/Sprite.h"
#include "FontEnum.h"

class DispString
{
private:
	DispString();
	~DispString();

public:
	static DispString& Instance()
	{
		static DispString instance;
		return instance;
	}

	void Draw(const TCHAR* str, DirectX::XMFLOAT2 position, float size, TEXT_ALIGN align = TEXT_ALIGN::UPPER_LEFT, DirectX::XMFLOAT4 color = {1,1,1,1}, bool outline = false, DirectX::XMFLOAT4 outlineColor = {0,0,0,1}, float outlineOffset = 1);
	
private:
	void Render(float dx, float dy, float dw, float dh, float r, float g, float b, float a, float angle);

	float getBufferW(TEXT_ALIGN align, float width);
	float getBufferH(TEXT_ALIGN align, float height);

private:
	std::map<UINT, FontTexture*> dictionary;

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
};