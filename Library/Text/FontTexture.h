#pragma once

#include <wrl.h>
#include <d3d11.h>
#include <map>
#include <string>

// 文字テクスチャを作成するクラス

//--------------------------------------------------------------
//  FontTexture
//--------------------------------------------------------------
class FontTexture
{
private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	D3D11_TEXTURE2D_DESC texture2dDesc;
public:
	FontTexture();
	~FontTexture();
	void Create(TCHAR* c, int fontSize);	// 文字テクスチャの作成
	ID3D11ShaderResourceView* GetShaderResourceView() { return shaderResourceView.Get(); }
	D3D11_TEXTURE2D_DESC GetTexture2dDesc() { return texture2dDesc; }
};