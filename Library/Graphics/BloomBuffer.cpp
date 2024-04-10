#include "BloomBuffer.h"
#include "Graphics.h"
#include "../ErrorLogger.h"
#include "../ImGui/ImGuiManager.h"

BloomBuffer::BloomBuffer(uint32_t width, uint32_t height)
{
	Graphics& gfx = Graphics::Instance();

	HRESULT hr = S_OK;

	// -------- RTV çÏê¨ --------

	//  allRTV 
	Microsoft::WRL::ComPtr<ID3D11Texture2D> allRenderTargetBuffer;
	D3D11_TEXTURE2D_DESC allTexture2dDesc{};
	allTexture2dDesc.Width = width;
	allTexture2dDesc.Height = height;
	allTexture2dDesc.MipLevels = 1;
	allTexture2dDesc.ArraySize = 1;
	allTexture2dDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	allTexture2dDesc.SampleDesc.Count = 1;
	allTexture2dDesc.SampleDesc.Quality = 0;
	allTexture2dDesc.Usage = D3D11_USAGE_DEFAULT;
	allTexture2dDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	allTexture2dDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	allTexture2dDesc.MiscFlags = 0;
	hr = gfx.device->CreateTexture2D(&allTexture2dDesc, 0, allRenderTargetBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	D3D11_RENDER_TARGET_VIEW_DESC allRenderTargetViewDesc{};
	allRenderTargetViewDesc.Format = allTexture2dDesc.Format;
	allRenderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	hr = gfx.device->CreateRenderTargetView(allRenderTargetBuffer.Get(), &allRenderTargetViewDesc, allRTV.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	// noLuminanceExtractRTV 
	Microsoft::WRL::ComPtr<ID3D11Texture2D> noLuminanceExtractRenderTargetBuffer;
	D3D11_TEXTURE2D_DESC noLuminanceExtractTexture2dDesc{};
	noLuminanceExtractTexture2dDesc.Width = width;
	noLuminanceExtractTexture2dDesc.Height = height;
	noLuminanceExtractTexture2dDesc.MipLevels = 1;
	noLuminanceExtractTexture2dDesc.ArraySize = 1;
	noLuminanceExtractTexture2dDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	noLuminanceExtractTexture2dDesc.SampleDesc.Count = 1;
	noLuminanceExtractTexture2dDesc.SampleDesc.Quality = 0;
	noLuminanceExtractTexture2dDesc.Usage = D3D11_USAGE_DEFAULT;
	noLuminanceExtractTexture2dDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	noLuminanceExtractTexture2dDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	noLuminanceExtractTexture2dDesc.MiscFlags = 0;
	hr = gfx.device->CreateTexture2D(&noLuminanceExtractTexture2dDesc, 0, noLuminanceExtractRenderTargetBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	D3D11_RENDER_TARGET_VIEW_DESC noLuminanceExtractRenderTargetViewDesc{};
	noLuminanceExtractRenderTargetViewDesc.Format = noLuminanceExtractTexture2dDesc.Format;
	noLuminanceExtractRenderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	hr = gfx.device->CreateRenderTargetView(noLuminanceExtractRenderTargetBuffer.Get(), &noLuminanceExtractRenderTargetViewDesc, noLuminanceExtractRTV.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	// -------- SRV çÏê¨ --------

	// allSRV
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
	shaderResourceViewDesc.Format = allTexture2dDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;
	hr = gfx.device->CreateShaderResourceView(allRenderTargetBuffer.Get(), &shaderResourceViewDesc, shaderResourceViews[0].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	// noLuminanceExtractSRV
	hr = gfx.device->CreateShaderResourceView(noLuminanceExtractRenderTargetBuffer.Get(), &shaderResourceViewDesc, shaderResourceViews[1].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	// -------- DSV çÏê¨ --------

	allTexture2dDesc.CPUAccessFlags = 0;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	allTexture2dDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	allTexture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	hr = gfx.device->CreateTexture2D(&allTexture2dDesc, 0, depthStencilBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Flags = 0;
	hr = gfx.device->CreateDepthStencilView(depthStencilBuffer.Get(), &depthStencilViewDesc, depthStencilView.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	// -------- SRV çÏê¨ --------
	shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	hr = gfx.device->CreateShaderResourceView(depthStencilBuffer.Get(), &shaderResourceViewDesc, shaderResourceViews[3].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));


	// ------- viewport -------
	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
}

void BloomBuffer::Clear(float r, float g, float b, float a, float depth)
{
	Graphics& gfx = Graphics::Instance();

	float color[4] = { r,g,b,a };
	gfx.deviceContext->ClearRenderTargetView(allRTV.Get(), color);
	gfx.deviceContext->ClearRenderTargetView(noLuminanceExtractRTV.Get(), color);
	gfx.deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, 0);
}

void BloomBuffer::Activate()
{
	Graphics& gfx = Graphics::Instance();

	viewportCount = D3D10_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	gfx.deviceContext->RSGetViewports(&viewportCount, cachedViewports);
	gfx.deviceContext->OMGetRenderTargets(1, cachedRenderTargetView.ReleaseAndGetAddressOf(), cachedDepthStencilView.ReleaseAndGetAddressOf());
	gfx.deviceContext->RSSetViewports(1, &viewport);

	ID3D11RenderTargetView* rtv[2] = {
		allRTV.Get(),
		noLuminanceExtractRTV.Get(),
	};

	gfx.deviceContext->OMSetRenderTargets(2, rtv, depthStencilView.Get());
}

void BloomBuffer::DeActivate()
{
	Graphics& gfx = Graphics::Instance();

	gfx.deviceContext->RSSetViewports(viewportCount, cachedViewports);
	gfx.deviceContext->OMSetRenderTargets(1, cachedRenderTargetView.GetAddressOf(), cachedDepthStencilView.Get());
}