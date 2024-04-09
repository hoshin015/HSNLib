#pragma once
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>

class LineRenderer
{
private:
	LineRenderer() {};
	~LineRenderer() {};

public:
	static LineRenderer& Instance()
	{
		static LineRenderer instance;
		return instance;
	}

	// èâä˙âª
	void Initialize();

	// ï`âÊé¿çs
	void Render();
 
	// í∏ì_í«â¡
	void AddVertex(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4& color);

private:
	struct Vertex
	{
		DirectX::XMFLOAT4	color;
		DirectX::XMFLOAT3	position;
	};


	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader>		vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>		inputLayout;

	Microsoft::WRL::ComPtr<ID3D11BlendState>		blendState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	depthStencilState;

	std::vector<Vertex>			vertices;
	UINT						capacity = 1024;
};