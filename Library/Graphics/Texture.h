#pragma once
#include <WICTextureLoader.h>
using namespace DirectX;

#include <wrl.h>
using namespace Microsoft::WRL;

#include <string>
#include <map>
using namespace std;

static map<wstring, ComPtr<ID3D11ShaderResourceView>> resources;
HRESULT LoadTextureFromFile(const wchar_t* filename, ID3D11ShaderResourceView** shaderResourceView, D3D11_TEXTURE2D_DESC* texture2dDesc);
HRESULT LoadTextureFromFile(const wchar_t* filename, ID3D11ShaderResourceView** shaderResourceView, D3D11_TEXTURE2D_DESC* texture2dDesc, ID3D11Texture2D** texture);
void ReleaseAllTextures();