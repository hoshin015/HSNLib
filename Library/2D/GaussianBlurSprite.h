//#pragma once
//#include <d3d11.h>
//#include <DirectXMath.h>
//#include "../Graphics/Texture.h"
//#include "../Graphics/Graphics.h"
//
//class GaussianBlurSprite
//{
//public:
//	GaussianBlurSprite(const wchar_t* filename);
//	~GaussianBlurSprite();
//	void Render(float dx, float dy, float dw, float dh, float r, float g, float b, float a, float angle);
//	void Render(float dx, float dy, float dw, float dh, float r, float g, float b, float a, float angle, float sx, float sy, float sw, float sh);
//
//private:
//	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
//	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
//	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
//	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
//	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
//	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
//	D3D11_TEXTURE2D_DESC texture2dDesc;
//
//	struct vertex
//	{
//		DirectX::XMFLOAT3 position;
//		DirectX::XMFLOAT4 color;
//		DirectX::XMFLOAT2 texcoord;
//	};
//
//	struct CBFilter
//	{
//		DirectX::XMFLOAT4 weights[Graphics::MaxKernelSize];
//		float kernelSize;
//		DirectX::XMFLOAT2 texcel;
//		float pad;
//	};
//
//	// フィルター値計算
//	void CalcGaussianFilter(CBFilter& cbFilter, const Graphics::GaussianFilterData& gaussianFilterData);
//};