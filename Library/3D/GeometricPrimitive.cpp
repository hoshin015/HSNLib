#include <memory>
#include <vector>
#include "GeometricPrimitive.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/Shader.h"
#include "../ErrorLogger.h"

GeometricPrimitive::GeometricPrimitive()
{
	// --- Graphics 取得 ---
	Graphics& gfx = Graphics::Instance();

	HRESULT hr = S_OK;

	//--- 頂点シェーダーオブジェクトと入力レイアウトオブジェクトの生成 ---
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	CreateVsFromCso("./Data/Shader/GeometricPrimitive_VS.cso", vertexShader.GetAddressOf(), inputLayout.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));

	//--- ピクセルシェーダーオブジェクトの生成 ---
	CreatePsFromCso("./Data/Shader/GeometricPrimitive_PS.cso", pixelShader.GetAddressOf());

	//--- 定数バッファオブジェクトの生成 ---
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth = sizeof(Constants);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = gfx.device->CreateBuffer(&bufferDesc, nullptr, constantBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
}

void GeometricPrimitive::Render(const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& materialColor)
{
	// --- Graphics 取得 ---
	Graphics& gfx = Graphics::Instance();

	uint32_t stride = sizeof(Vertex);
	uint32_t offset = 0;

	gfx.deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	gfx.deviceContext->IASetInputLayout(inputLayout.Get());

	gfx.deviceContext->VSSetShader(vertexShader.Get(), nullptr, 0);
	gfx.deviceContext->PSSetShader(pixelShader.Get(), nullptr, 0);

	Constants data = { world, materialColor };
	gfx.deviceContext->UpdateSubresource(constantBuffer.Get(), 0, 0, &data, 0, 0);
	gfx.deviceContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

	gfx.deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	gfx.deviceContext->Draw(vertexCount, 0);
}
