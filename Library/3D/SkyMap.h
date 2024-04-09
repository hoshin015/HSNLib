#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>


class SkyMap
{
public:
	SkyMap(const wchar_t* filename);
	~SkyMap();

	void Render();

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;

	struct Constants
	{
		DirectX::XMFLOAT4X4 inverseViewProjection;
	};
};