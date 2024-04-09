#include <sstream>
#include "GraphicsSpriteBatch.h"
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

GraphicsSpriteBatch::GraphicsSpriteBatch(const wchar_t* filename, size_t masSprites) : maxVertices(masSprites * 6)
{
	Graphics& gfx = Graphics::Instance();

	//--- < 頂点情報のセット > ---
	std::unique_ptr<vertex[]> vertices{ std::make_unique<vertex[]>(maxVertices) };

	//--- < 頂点バッファオブジェクトの生成 > ---
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth = sizeof(vertex) * maxVertices;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA subresourceData{};
	subresourceData.pSysMem = vertices.get();
	subresourceData.SysMemPitch = 0;
	subresourceData.SysMemSlicePitch = 0;
	HRESULT hr = gfx.device->CreateBuffer(&bufferDesc, &subresourceData, vertexBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	//--- < 頂点シェーダーオブジェクトと入力レイアウトオブジェクトの生成 > ---
	const char* csoName =  "./Data/Shader/Sprite_VS.cso";

	D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	CreateVsFromCso(csoName, vertexShader.GetAddressOf(), inputLayout.GetAddressOf(), inputElementDesc, _countof(inputElementDesc));

	//--- < ピクセルシェーダーオブジェクトの生成 > ---
	csoName = "./Data/Shader/Sprite_Ps.cso";
	CreatePsFromCso(csoName, pixelShader.GetAddressOf());

	//--- < 画像ファイルのロードとshaderResourceViewの生成とテクスチャ情報の取得 > ---
	LoadTextureFromFile(filename, shaderResourceView.GetAddressOf(), &texture2dDesc);
}

GraphicsSpriteBatch::~GraphicsSpriteBatch()
{
}

void GraphicsSpriteBatch::render(float dx, float dy, float dw, float dh)
{
	render(dx, dy, dw, dh, 1, 1, 1, 1, 0, 0, 0, texture2dDesc.Width, texture2dDesc.Height);

}

void GraphicsSpriteBatch::render(float dx, float dy, float dw, float dh, float r, float g, float b, float a, float angle)
{
	render(dx, dy, dw, dh, r, g, b, a, angle, 0, 0, texture2dDesc.Width, texture2dDesc.Height);
}

void GraphicsSpriteBatch::render(float dx, float dy, float dw, float dh, float r, float g, float b, float a, float angle, float sx, float sy, float sw, float sh)
{
	Graphics& gfx = Graphics::Instance();

	//--- < スクリーン(ビューポート)のサイズを取得 > ---
	D3D11_VIEWPORT viewport{};
	UINT numViewports = 1;
	gfx.deviceContext->RSGetViewports(&numViewports, &viewport);

	//--- < 矩形の各頂点の位置(スクリーン座標系)を計算する > ---

	// left top
	float x0 = dx;
	float y0 = dy;
	// right top
	float x1 = dx + dw;
	float y1 = dy;
	// left bottom
	float x2 = dx;
	float y2 = dy + dh;
	// right bottom
	float x3 = dx + dw;
	float y3 = dy + dh;

	//--- < 頂点回転処理 > ---

	// 回転の中心を矩形の中心点にした場合
	float cx = dx + dw * 0.5f;
	float cy = dy + dh * 0.5f;

	float cos = cosf(DirectX::XMConvertToRadians(angle));
	float sin = sinf(DirectX::XMConvertToRadians(angle));

	Rotate(x0, y0, cx, cy, cos, sin);
	Rotate(x1, y1, cx, cy, cos, sin);
	Rotate(x2, y2, cx, cy, cos, sin);
	Rotate(x3, y3, cx, cy, cos, sin);


	//--- < スクリーン座標系からNDCへ座標変換 > ---
	x0 = 2.0f * x0 / viewport.Width - 1.0f;
	y0 = 1.0f - 2.0f * y0 / viewport.Height;
	x1 = 2.0f * x1 / viewport.Width - 1.0f;
	y1 = 1.0f - 2.0f * y1 / viewport.Height;
	x2 = 2.0f * x2 / viewport.Width - 1.0f;
	y2 = 1.0f - 2.0f * y2 / viewport.Height;
	x3 = 2.0f * x3 / viewport.Width - 1.0f;
	y3 = 1.0f - 2.0f * y3 / viewport.Height;


	//--- < テクセル座標系からUV座標系への変換 > ---
	float u0 = sx / texture2dDesc.Width;
	float v0 = sy / texture2dDesc.Height;
	float u1 = (sx + sw) / texture2dDesc.Width;
	float v1 = (sy + sh) / texture2dDesc.Height;

	//--- < 頂点情報をpush > ---
	vertices.push_back({ { x0, y0, 0}, { r, g, b, a}, { u0, v0} });
	vertices.push_back({ { x1, y1, 0}, { r, g, b, a}, { u1, v0} });
	vertices.push_back({ { x2, y2, 0}, { r, g, b, a}, { u0, v1} });
	vertices.push_back({ { x2, y2, 0}, { r, g, b, a}, { u0, v1} });
	vertices.push_back({ { x1, y1, 0}, { r, g, b, a}, { u1, v0} });
	vertices.push_back({ { x3, y3, 0}, { r, g, b, a}, { u1, v1} });


}

void GraphicsSpriteBatch::begin()
{
	Graphics& gfx = Graphics::Instance();

	//--- < 頂点情報のクリア > ---
	vertices.clear();

	//--- < シェーダーのバインド > ---
	gfx.deviceContext->VSSetShader(vertexShader.Get(), nullptr, 0);
	gfx.deviceContext->PSSetShader(pixelShader.Get(), nullptr, 0);

	//--- < シェーダーリソースのバインド > ---
	gfx.deviceContext->PSSetShaderResources(0, 1, shaderResourceView.GetAddressOf());
}

void GraphicsSpriteBatch::end()
{
	Graphics& gfx = Graphics::Instance();

	//--- < 頂点情報を元に頂点バッファオブジェクトを更新する > ---
	HRESULT hr = S_OK;
	D3D11_MAPPED_SUBRESOURCE mappedSubresource{};
	hr = gfx.deviceContext->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);

	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	size_t vertexCount = vertices.size();
	_ASSERT_EXPR(maxVertices >= vertexCount, "Buffer overflow");
	vertex* data = reinterpret_cast<vertex*>(mappedSubresource.pData);
	if (data != nullptr)
	{
		const vertex* p = vertices.data();
		memcpy_s(data, maxVertices * sizeof(vertex), p, vertexCount * sizeof(vertex));
	}
	gfx.deviceContext->Unmap(vertexBuffer.Get(), 0);

	//--- < 頂点バッファーのバインド > ---
	UINT stride = sizeof(vertex);
	UINT offset = 0;
	gfx.deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	//--- < プリミティブタイプおよにデータ順序に関する情報のバインド > ---
	gfx.deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//--- < 入力レイアウトオブジェクトのバインド > ---
	gfx.deviceContext->IASetInputLayout(inputLayout.Get());

	//--- < プリミティブの描画 > ---
	gfx.deviceContext->Draw(static_cast<UINT>(vertexCount), 0);
}
