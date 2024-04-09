#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "../Graphics/Texture.h"

class MaskSprite
{
public:
	MaskSprite(const wchar_t* filename, const wchar_t* filenameMask);
	~MaskSprite();
	void Render(float dx, float dy, float dw, float dh, float r, float g, float b, float a, float angle);
	void Render(float dx, float dy, float dw, float dh, float r, float g, float b, float a, float angle, float sx, float sy, float sw, float sh);

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> maskSRV;
	D3D11_TEXTURE2D_DESC texture2dDesc;

	struct vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texcoord;
	};

	struct MaskConstant
	{
		float dissolveThreshold;		// ディゾルブ量
		float edgeThreshold;			// 緑の閾値
		DirectX::XMFLOAT2 pad;			// ダミー
		DirectX::XMFLOAT4 edgeColor;	// 緑の色
	};
public:
	MaskConstant maskConstant;
};