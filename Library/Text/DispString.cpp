#include <iostream>
#include <tchar.h>
#include "DispString.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/Shader.h"
#include "../ErrorLogger.h"

//
//  参考 ： https://blog.jumtana.com/2007/09/vc-unicode.html#google_vignette
//  
// 文字列がすべて半角か確認する
BOOL IsAllHankaku(const TCHAR* pTarget, int nLen, int index)
{
	// 文字の種別を取得する
	WORD* pType = new WORD[nLen];
	GetStringTypeEx(
		LOCALE_SYSTEM_DEFAULT    // ユニコード版プロジェクトでは、無視される引数      
		, CT_CTYPE3                // 全角、半角、ひらがな等を判定するタイプ
		, pTarget                  // 判定対象となる文字配列
		, nLen                     // 判定対象となる文字配列のサイズ
		, pType                    // 判定結果の配列
	);
	// 結果がすべて半角なら TRUE をかえす
	BOOL isAllHankaku = TRUE;
	if (!(pType[index] & C3_HALFWIDTH)) {
		isAllHankaku = FALSE;
	}

	delete[] pType;
	return isAllHankaku;
}


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

// コンストラクタ
DispString::DispString()
{
	Graphics* gfx = &Graphics::Instance();

	//--- < 頂点情報のセット > ---
	vertex vertices[] =
	{
		{{ -0.5f,  0.5f, 0 }, { 1, 1, 1, 1 }, { 0, 0 }},
		{{  0.5f,  0.5f, 0 }, { 1, 0, 0, 1 }, { 1, 0 }},
		{{ -0.5f, -0.5f, 0 }, { 0, 1, 0, 1 }, { 0, 1 }},
		{{  0.5f, -0.5f, 0 }, { 0, 0, 1, 1 }, { 1, 1 }},
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

	//--- < 頂点シェーダーオブジェクトと入力レイアウトオブジェクトの生成 > ---
	const char* csoName{ "./Data/Shader/FontTexture_VS.cso" };

	D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	CreateVsFromCso(csoName, vertexShader.GetAddressOf(), inputLayout.GetAddressOf(), inputElementDesc, _countof(inputElementDesc));

	//--- < ピクセルシェーダーオブジェクトの生成 > ---
	csoName = { "./Data/Shader/FontTexture_PS.cso" };
	CreatePsFromCso(csoName, pixelShader.GetAddressOf());


	// ロケール設定
	std::wcin.imbue(std::locale("japanese"));
	std::wcout.imbue(std::locale("japanese"));
}

// デストラクタ
DispString::~DispString()
{
    auto iter = dictionary.begin();
    while (iter != dictionary.end())
    {
        delete iter->second;
        iter = dictionary.erase(iter);
    }
    dictionary.clear();
}

void DispString::Draw(const TCHAR* str, DirectX::XMFLOAT2 position, float size, TEXT_ALIGN align, DirectX::XMFLOAT4 color, bool outline, DirectX::XMFLOAT4 outlineColor, float outlineOffset)
{
    int num = _tcslen(str);

	float width = 0;
	float height = size;

	// --- 全角半角識別 ---
	std::wstring szTarget = std::wstring(str);

	for (int i = 0; i < num; ++i)
	{
		if (auto iter = dictionary.find((UINT)((TCHAR*)str[i])); iter != end(dictionary))
		{
			//exist
		}
		else
		{
			//not exists
			FontTexture* font = new FontTexture();
			font->Create((TCHAR*)&str[i], size);
			dictionary.emplace((UINT)((TCHAR*)str[i]), font);
		}

		// 全角半角チェック
		BOOL isAllHan = IsAllHankaku(szTarget.c_str(), szTarget.length(), i);
		width += (isAllHan) ? (size * 0.5f) : (size);
	}	

	float bufferW = getBufferW(align, width);		//アラインメントによる横幅調整値
	float bufferH = getBufferH(align, height);		//アラインメントによる縦幅調整値

	width = 0;
	for (int i = 0; i < num; ++i)
	{
		// ここからテクスチャ描画処理
		shaderResourceView = dictionary[(UINT)str[i]]->GetShaderResourceView();
		texture2dDesc = dictionary[(UINT)str[i]]->GetTexture2dDesc();

		// 全角半角チェック
		BOOL isAllHan = IsAllHankaku(szTarget.c_str(), szTarget.length(), i);
		float addWidth = (isAllHan) ? (size * 0.5f) : (size);

		// アウトライン描画
		if (outline)
		{
			Render(position.x + width + bufferW - outlineOffset, position.y + bufferH - outlineOffset, addWidth, size, outlineColor.x, outlineColor.y, outlineColor.z, outlineColor.w, 0);
			Render(position.x + width + bufferW + outlineOffset, position.y + bufferH - outlineOffset, addWidth, size, outlineColor.x, outlineColor.y, outlineColor.z, outlineColor.w, 0);
			Render(position.x + width + bufferW - outlineOffset, position.y + bufferH + outlineOffset, addWidth, size, outlineColor.x, outlineColor.y, outlineColor.z, outlineColor.w, 0);
			Render(position.x + width + bufferW + outlineOffset, position.y + bufferH + outlineOffset, addWidth, size, outlineColor.x, outlineColor.y, outlineColor.z, outlineColor.w, 0);
		}
		
		
		Render(position.x + width + bufferW, position.y + bufferH, addWidth, size, color.x, color.y, color.z, color.w, 0);
		width += addWidth;
	}
}

void DispString::Render(float dx, float dy, float dw, float dh, float r, float g, float b, float a, float angle)
{
	Graphics* gfx = &Graphics::Instance();

	//--- < スクリーン(ビューポート)のサイズを取得 > ---
	D3D11_VIEWPORT viewport{};
	UINT numViewports{ 1 };
	gfx->deviceContext->RSGetViewports(&numViewports, &viewport);

	//--- < 矩形の各頂点の位置(スクリーン座標系)を計算する > ---

	// left top
	float x0{ dx };
	float y0{ dy };
	// right top
	float x1{ dx + dw };
	float y1{ dy };
	// left bottom
	float x2{ dx };
	float y2{ dy + dh };
	// right bottom
	float x3{ dx + dw };
	float y3{ dy + dh };

	//--- < 頂点回転処理 > ---

	// 回転の中心を矩形の中心点にした場合
	float cx = dx + dw * 0.5f;
	float cy = dy + dh * 0.5f;

	float cos{ cosf(DirectX::XMConvertToRadians(angle)) };
	float sin{ sinf(DirectX::XMConvertToRadians(angle)) };

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

	//--- < 計算結果で頂点バッファオブジェクトを更新する > ---
	HRESULT hr{ S_OK };
	D3D11_MAPPED_SUBRESOURCE mappedSubresource{};
	hr = gfx->deviceContext->Map(vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	vertex* vertices{ reinterpret_cast<vertex*>(mappedSubresource.pData) };
	if (vertices != nullptr)
	{
		vertices[0].position = { x0 ,y0, 0 };
		vertices[1].position = { x1 ,y1, 0 };
		vertices[2].position = { x2 ,y2, 0 };
		vertices[3].position = { x3 ,y3, 0 };
		vertices[0].color = vertices[1].color = vertices[2].color = vertices[3].color = { r,g,b,a };

		vertices[0].texcoord = { 0, 0 };
		vertices[1].texcoord = { 1, 0 };
		vertices[2].texcoord = { 0, 1 };
		vertices[3].texcoord = { 1, 1 };
	}
	gfx->deviceContext->Unmap(vertexBuffer.Get(), 0);

	//--- < 頂点バッファーのバインド > ---
	UINT stride{ sizeof(vertex) };
	UINT offset{ 0 };
	gfx->deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);

	//--- < プリミティブタイプおよにデータ順序に関する情報のバインド > ---
	gfx->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//--- < 入力レイアウトオブジェクトのバインド > ---
	gfx->deviceContext->IASetInputLayout(inputLayout.Get());

	//--- < シェーダーのバインド > ---
	gfx->deviceContext->VSSetShader(vertexShader.Get(), nullptr, 0);
	gfx->deviceContext->PSSetShader(pixelShader.Get(), nullptr, 0);

	//--- < シェーダーリソースのバインド > ---
	gfx->deviceContext->PSSetShaderResources(0, 1, shaderResourceView.GetAddressOf());

	//--- < プリミティブの描画 > ---
	gfx->deviceContext->Draw(4, 0);
}

//横幅調整用
float DispString::getBufferW(TEXT_ALIGN align, float width)
{
	switch (align)
	{
	case TEXT_ALIGN::UPPER_LEFT:
	case TEXT_ALIGN::MIDDLE_LEFT:
	case TEXT_ALIGN::LOWER_LEFT:
		return 0.0f;
		break;
	case TEXT_ALIGN::UPPER_MIDDLE:
	case TEXT_ALIGN::MIDDLE:
	case TEXT_ALIGN::LOWER_MIDDLE:
		return -width * 0.5f;
		break;
	case TEXT_ALIGN::UPPER_RIGHT:
	case TEXT_ALIGN::MIDDLE_RIGHT:
	case TEXT_ALIGN::LOWER_RIGHT:
		return -width;
	}
	return 0.0f;
}

//縦幅調整用
float DispString::getBufferH(TEXT_ALIGN align, float height)
{
	switch (align)
	{
	case TEXT_ALIGN::UPPER_LEFT:
	case TEXT_ALIGN::UPPER_MIDDLE:
	case TEXT_ALIGN::UPPER_RIGHT:
		return 0.0f;
		break;
	case TEXT_ALIGN::MIDDLE_LEFT:
	case TEXT_ALIGN::MIDDLE:
	case TEXT_ALIGN::MIDDLE_RIGHT:
		return -height * 0.5f;
		break;
	case TEXT_ALIGN::LOWER_LEFT:
	case TEXT_ALIGN::LOWER_MIDDLE:
	case TEXT_ALIGN::LOWER_RIGHT:
		return -height;
	}
	return 0.0f;
}


