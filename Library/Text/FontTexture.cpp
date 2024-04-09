#include "FontTexture.h"
#include "../Graphics/Graphics.h"
#include "../ErrorLogger.h"



// コンストラクタ
FontTexture::FontTexture()
{
	DESIGNVECTOR design;
	AddFontResourceEx(
		TEXT("Data/Font/MSGOTHIC.TTF"),
		FR_PRIVATE,
		&design
	);
}

// デストラクタ
FontTexture::~FontTexture()
{
	DESIGNVECTOR design;
	RemoveFontResourceEx(
		TEXT("Data/Font/MSGOTHIC.TTF"),
		FR_PRIVATE,
		&design
	);
}

// テクスチャデータ作成
void FontTexture::Create(TCHAR* c, int fontSize)
{
	// Graphics 取得
	Graphics& gfx = Graphics::Instance();

	HRESULT hr;
	// フォントの生成
	LOGFONT lf = {
		fontSize,
		0,
		0, 
		0, 
		0, 
		0,
		0,
		0, 
		SHIFTJIS_CHARSET,
		OUT_TT_ONLY_PRECIS,
		CLIP_DEFAULT_PRECIS, 
		PROOF_QUALITY,
		FIXED_PITCH | FF_MODERN,
		TEXT("MSGOTHIC")
	};
	//TEXT("あんずもじ湛")
	//TEXT("HG創英角ﾎﾟｯﾌﾟ体")
	//TEXT("HG行書体")

	HFONT hFont = CreateFontIndirect( &lf );
	if( !( hFont ) )
	{
	}

	// デバイスコンテキスト取得
	// デバイスにフォントを持たせないとGetGlyphOutline関数はエラーとなる
	HDC hdc = GetDC( NULL );
	HFONT oldFont = (HFONT)SelectObject( hdc, hFont );

	// 文字コード取得
	UINT code = 0;
	#if _UNICODE
	// unicodeの場合、文字コードは単純にワイド文字のUINT変換です
		code = (UINT)*c;
	#else
	// マルチバイト文字の場合、
	// 1バイト文字のコードは1バイト目のUINT変換、
	// 2バイト文字のコードは[先導コード]*256 + [文字コード]です
	if(IsDBCSLeadByte(*c))
		code = (BYTE)c[0]<<8 | (BYTE)c[1];
	else
		code = c[0];
	#endif

	// フォントビットマップ取得
	TEXTMETRIC TM;
	GetTextMetrics( hdc, &TM );
	GLYPHMETRICS GM;
	CONST MAT2 Mat = { {0,1}, {0,0}, {0,0}, {0,1} };
	DWORD size = GetGlyphOutline( hdc, code, GGO_GRAY8_BITMAP, &GM, 0, NULL, &Mat );
	BYTE* ptr = new BYTE[ size ];
	GetGlyphOutline( hdc, code, GGO_GRAY8_BITMAP, &GM, size, ptr, &Mat );

	// デバイスコンテキストとフォントハンドルの開放
	SelectObject( hdc, oldFont );
	DeleteObject( hFont );
	ReleaseDC( NULL, hdc );


	//--------------------------------
	// 書き込み可能テクスチャ作成
	//--------------------------------

	// CPUで書き込みができるテクスチャを作成

	// テクスチャ作成
	D3D11_TEXTURE2D_DESC desc;
	memset( &desc, 0, sizeof( desc ) );
	desc.Width			= GM.gmCellIncX;
	desc.Height			= TM.tmHeight;
	desc.MipLevels		= 1;
	desc.ArraySize		= 1;
	desc.Format			= DXGI_FORMAT_R8G8B8A8_UNORM;	// RGBA(255,255,255,255)タイプ
	desc.SampleDesc.Count= 1;
	desc.Usage			= D3D11_USAGE_DYNAMIC;			// 動的（書き込みするための必須条件）
	desc.BindFlags		= D3D11_BIND_SHADER_RESOURCE;	// シェーダリソースとして使う
	desc.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE;	// CPUからアクセスして書き込みOK

	ID3D11Texture2D* tex2D;					///< 2Ｄテクスチャ
	hr = gfx.device->CreateTexture2D( &desc, 0, &tex2D );
	
	D3D11_MAPPED_SUBRESOURCE hMappedResource{};
	if (tex2D != nullptr)
	{
		hr = gfx.deviceContext->Map(
			tex2D,
			0,
			D3D11_MAP_WRITE_DISCARD,
			0,
			&hMappedResource);
		// ここで書き込む
		BYTE* pBits = (BYTE*)hMappedResource.pData;

		// フォント情報の書き込み
		// iOfs_x, iOfs_y : 書き出し位置(左上)
		// iBmp_w, iBmp_h : フォントビットマップの幅高
		// Level : α値の段階 (GGO_GRAY8_BITMAPなので65段階)
		int iOfs_x = GM.gmptGlyphOrigin.x;
		int iOfs_y = TM.tmAscent - GM.gmptGlyphOrigin.y;
		int iBmp_w = GM.gmBlackBoxX + (4 - (GM.gmBlackBoxX % 4)) % 4;
		int iBmp_h = GM.gmBlackBoxY;
		int Level = 65;
		int x, y;
		DWORD Alpha, Color;
		memset(pBits, 0, hMappedResource.RowPitch * TM.tmHeight);
		for (y = iOfs_y; y < iOfs_y + iBmp_h; y++)
		{
			for (x = iOfs_x; x < iOfs_x + iBmp_w; x++)
			{
				Alpha = (255 * ptr[x - iOfs_x + iBmp_w * (y - iOfs_y)]) / (Level - 1);
				Color = 0x00ffffff | (Alpha << 24);

				memcpy((BYTE*)pBits + hMappedResource.RowPitch * y + 4 * x, &Color, sizeof(DWORD));
			}
		}

		gfx.deviceContext->Unmap(tex2D, 0);
	}

	// テクスチャ情報を取得する
	tex2D->GetDesc(&texture2dDesc);

	// shaderResourceView の情報を作成する
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
	shaderResourceViewDesc.Format = texture2dDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = texture2dDesc.MipLevels;

	// ShaderResourceViewを作成する
	hr = gfx.device->CreateShaderResourceView(tex2D, &shaderResourceViewDesc, &shaderResourceView);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));


	delete[] ptr;
}

