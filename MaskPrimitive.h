#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "Library/Graphics/Texture.h"

class MaskPrimitive {
public:
	MaskPrimitive();
	~MaskPrimitive();

	void Render(float dx, float dy, float dw, float dh, float r, float g, float b, float a, float angle, float radius = 0, DirectX::XMFLOAT2 circlePos = {}, int blend = 0);

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
	Microsoft::WRL::ComPtr<ID3D11BlendState> blendState[2];

	struct vertex {
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT2 texcoord;
	};

	struct CBuffer {
		DirectX::XMFLOAT2 circlePos;
		float radius;
		float pad;
	};
};