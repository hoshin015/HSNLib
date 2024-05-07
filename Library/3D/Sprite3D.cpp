#include <sstream>
#include "Sprite3D.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/Shader.h"
#include "../ErrorLogger.h"

// 回転
inline void  Rotate(float& x, float& y, float cx, float cy, float cos, float sin)
{
	x -= cx;
	y -= cy;

	float tx{ x }, ty{ y };
	x = cos * tx + -sin * ty;
	y = sin * tx + cos * ty;

	x += cx;
	y += cy;
};

Sprite3D::Sprite3D(const wchar_t* filename)
{
	Graphics* gfx = &Graphics::Instance();

	//--- < 頂点情報のセット > ---
	Vertex vertices[] =
	{
		{{ -0.5f,  0.5f, 0 }, { 1, 1, 1, 1 }, { 0, 0 }},	// Top Left			- [0]
		{{  0.5f,  0.5f, 0 }, { 1, 0, 0, 1 }, { 1, 0 }},	// Top Right		- [1]
		{{ -0.5f, -0.5f, 0 }, { 0, 1, 0, 1 }, { 0, 1 }},	// Bottom Left		- [2]
		{{  0.5f, -0.5f, 0 }, { 0, 0, 1, 1 }, { 1, 1 }},	// Bottom Right		- [3]
	};

	DWORD indices[] =
	{
		0, 1, 2,
		3, 2, 1
	};

	//--- < 頂点バッファオブジェクトの生成 > ---
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth = sizeof(vertices);
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA subresourceData{};
	subresourceData.pSysMem = vertices;
	subresourceData.SysMemPitch = 0;
	subresourceData.SysMemSlicePitch = 0;
	HRESULT hr = gfx->device->CreateBuffer(&bufferDesc, &subresourceData, vertexBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	//--- < インデックスバッファオブジェクトの生成 > ---
	D3D11_BUFFER_DESC indexBufferDesc{};
	indexBufferDesc.ByteWidth = sizeof(vertices);
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexBufferData;
	indexBufferData.pSysMem = indices;
	hr = gfx->device->CreateBuffer(&indexBufferDesc, &indexBufferData, indexBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));


	//--- < 頂点シェーダーオブジェクトと入力レイアウトオブジェクトの生成 > ---
	const char* csoName{ "./Data/Shader/Sprite3D_VS.cso" };

	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = 
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	CreateVsFromCso(csoName, vertexShader.GetAddressOf(), inputLayout.GetAddressOf(), inputElementDesc, _countof(inputElementDesc));

	//--- < ピクセルシェーダーオブジェクトの生成 > ---
	csoName = { "./Data/Shader/Sprite3D_PS.cso" };
	CreatePsFromCso(csoName, pixelShader.GetAddressOf());

	//--- < 画像ファイルのロードとshaderResourceViewの生成とテクスチャ情報の取得 > ---
	LoadTextureFromFile(filename, shaderResourceView.GetAddressOf(), &texture2dDesc);

	//--- 定数バッファオブジェクトの生成 ---
	D3D11_BUFFER_DESC constantBufferDesc{};
	constantBufferDesc.ByteWidth = sizeof(Constants);
	constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = gfx->device->CreateBuffer(&constantBufferDesc, nullptr, constantBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

}

Sprite3D::~Sprite3D()
{
}

void Sprite3D::Render(const DirectX::XMFLOAT4X4& world, DirectX::XMFLOAT4 color)
{
	Render(world, color, 0, 0, texture2dDesc.Width, texture2dDesc.Height);
}

void Sprite3D::Render(const DirectX::XMFLOAT4X4& world, DirectX::XMFLOAT4 color, float sx, float sy, float sw, float sh)
{
	Graphics* gfx = &Graphics::Instance();

	//TODO:1対1にするなら必要ないので消した
	//float dw = sw;
	//float dh = sh;

	//float w = 1;
	//float h = 1;

	//// 横幅のほうが大きい場合
	//if (dw > dh)
	//{
	//	w = 1;
	//	h = dh / dw;
	//}
	//// 縦幅のほうが大きい場合
	//else
	//{
	//	h = 1;
	//	w = dw / dh;
	//}

	
	//float x0 = (0 - w * 0.5f) * 2;
	//float y0 = (0 - h * 0.5f) * -2;
	//float x1 = (0 + w * 0.5f) * 2;
	//float y1 = (0 - h * 0.5f) * -2;
	//float x2 = (0 - w * 0.5f) * 2;
	//float y2 = (0 + h * 0.5f) * -2;
	//float x3 = (0 + w * 0.5f) * 2;
	//float y3 = (0 + h * 0.5f) * -2;
	float x0 = -.5f;
	float y0 = .5f;
	float x1 = .5f;
	float y1 = .5f;
	float x2 = -.5f;
	float y2 = -.5f;
	float x3 = .5f;
	float y3 = -.5f;

	//--- < テクセル座標系からUV座標系への変換 > ---
	float u0 = sx / texture2dDesc.Width;
	float v0 = sy / texture2dDesc.Height;
	float u1 = (sx + sw) / texture2dDesc.Width;
	float v1 = (sy + sh) / texture2dDesc.Height;

	//--- < 計算結果で頂点バッファオブジェクトを更新する > ---
	HRESULT hr{ S_OK };
	D3D11_MAPPED_SUBRESOURCE mappedSubresource{};
	hr = gfx->deviceContext->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	Vertex* vertices{ reinterpret_cast<Vertex*>(mappedSubresource.pData) };
	if (vertices != nullptr)
	{
		vertices[0].position = { x0 ,y0, 0 };
		vertices[1].position = { x1 ,y1, 0 };
		vertices[2].position = { x2 ,y2, 0 };
		vertices[3].position = { x3 ,y3, 0 };
		vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = color;

		vertices[0].texcoord = { u0,v0 };
		vertices[1].texcoord = { u1,v0 };
		vertices[2].texcoord = { u0,v1 };
		vertices[3].texcoord = { u1,v1 };
	}
	gfx->deviceContext->Unmap(vertexBuffer.Get(), 0);

	//--- < ラスタライザのバインド > ---
	gfx->SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_CULL_NONE);
	
	//--- < 頂点バッファーのバインド > ---
	UINT stride{ sizeof(Vertex) };
	UINT offset{ 0 };
	gfx->deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	gfx->deviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	//--- < プリミティブタイプおよにデータ順序に関する情報のバインド > ---
	gfx->deviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//--- < 入力レイアウトオブジェクトのバインド > ---
	gfx->deviceContext->IASetInputLayout(inputLayout.Get());

	//--- < シェーダーのバインド > ---
	gfx->deviceContext->VSSetShader(vertexShader.Get(), nullptr, 0);
	gfx->deviceContext->PSSetShader(pixelShader.Get(), nullptr, 0);

	Constants data = { world };
	gfx->deviceContext->UpdateSubresource(constantBuffer.Get(), 0, 0, &data, 0, 0);
	gfx->deviceContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

	//--- < シェーダーリソースのバインド > ---
	gfx->deviceContext->PSSetShaderResources(0, 1, shaderResourceView.GetAddressOf());

	//--- < プリミティブの描画 > ---
	gfx->deviceContext->DrawIndexed(6, 0, 0);
}
