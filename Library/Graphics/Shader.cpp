#include <cstdio>
#include <memory>
#include "Shader.h"
#include "Graphics.h"
#include "../ErrorLogger.h"

HRESULT CreateVsFromCso(const char* csoName, ID3D11VertexShader** vertexShader, ID3D11InputLayout** inputLayout, D3D11_INPUT_ELEMENT_DESC* inputElementDesc, UINT numElements)
{
	// --- Graphics の取得 ---
	Graphics& gfx = Graphics::Instance();

	HRESULT hr = S_OK;

	//--- 頂点シェーダーオブジェクトの生成 ---
	FILE* fp{};
	fopen_s(&fp, csoName, "rb");
	_ASSERT_EXPR(fp, "CSO File not Found");

	fseek(fp, 0, SEEK_END);
	long csoSz{ ftell(fp) };
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> csoData{ std::make_unique<unsigned char[]>(csoSz) };
	fread(csoData.get(), csoSz, 1, fp);
	fclose(fp);

	hr = gfx.device->CreateVertexShader(csoData.get(), csoSz, nullptr, vertexShader);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	//--- 入力レイアウトオブジェクトの生成 ---
	if (inputLayout)
	{
		hr = gfx.device->CreateInputLayout(inputElementDesc, numElements, csoData.get(), csoSz, inputLayout);
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	}

	return hr;
}

HRESULT CreatePsFromCso(const char* csoName, ID3D11PixelShader** pixelShader)
{
	// --- Graphics の取得 ---
	Graphics& gfx = Graphics::Instance();

	HRESULT hr = S_OK;

	//--- ピクセルシェーダーオブジェクトの生成 ---
	FILE* fp = {};
	fopen_s(&fp, csoName, "rb");
	_ASSERT_EXPR(fp, "CSO File not Found");

	fseek(fp, 0, SEEK_END);
	long csoSz = { ftell(fp) };
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> csoData = { std::make_unique<unsigned char[]>(csoSz) };
	fread(csoData.get(), csoSz, 1, fp);
	fclose(fp);

	hr = gfx.device->CreatePixelShader(csoData.get(), csoSz, nullptr, pixelShader);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	return hr;
}

HRESULT CreateGsFromCso(const char* cso_name, ID3D11GeometryShader** geometry_shader)
{
	// --- Graphics の取得 ---
	Graphics& gfx = Graphics::Instance();

	HRESULT hr = S_OK;
	
	FILE* fp = nullptr;
	fopen_s(&fp, cso_name, "rb");
	_ASSERT_EXPR(fp, "CSO File not Found");

	fseek(fp, 0, SEEK_END);
	long cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	hr = gfx.device->CreateGeometryShader(cso_data.get(), cso_sz, nullptr, geometry_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));
	
	return hr;
}

HRESULT CreateCsFromCso(const char* cso_name, ID3D11ComputeShader** compute_shader)
{
	// --- Graphics の取得 ---
	Graphics& gfx = Graphics::Instance();

	HRESULT hr = S_OK;
	
	FILE* fp = nullptr;
	fopen_s(&fp, cso_name, "rb");
	_ASSERT_EXPR(fp, "CSO File not Found");

	fseek(fp, 0, SEEK_END);
	long cso_sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	std::unique_ptr<unsigned char[]> cso_data = std::make_unique<unsigned char[]>(cso_sz);
	fread(cso_data.get(), cso_sz, 1, fp);
	fclose(fp);

	hr =gfx.device->CreateComputeShader(cso_data.get(), cso_sz, nullptr, compute_shader);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	return hr;
}
