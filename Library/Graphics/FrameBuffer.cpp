#include "FrameBuffer.h"
#include "Graphics.h"
#include "../ErrorLogger.h"
#include "../ImGui/ImGuiManager.h"

FrameBuffer::FrameBuffer(uint32_t width, uint32_t height)
{
	Graphics& gfx = Graphics::Instance();

	HRESULT hr = S_OK;

	// -------- RenderTarget [0]--------
	Microsoft::WRL::ComPtr<ID3D11Texture2D> renderTargetBuffer;
	D3D11_TEXTURE2D_DESC texture2dDesc{};
	texture2dDesc.Width = width;
	texture2dDesc.Height = height;
	texture2dDesc.MipLevels = 1;
	texture2dDesc.ArraySize = 1;
	texture2dDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	texture2dDesc.SampleDesc.Count = 1;
	texture2dDesc.SampleDesc.Quality = 0;
	texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
	texture2dDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texture2dDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texture2dDesc.MiscFlags = 0;
	hr = gfx.device->CreateTexture2D(&texture2dDesc, 0, renderTargetBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc{};
	renderTargetViewDesc.Format = texture2dDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	hr = gfx.device->CreateRenderTargetView(renderTargetBuffer.Get(), &renderTargetViewDesc, renderTargetViews[0].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	// -------- RenderTarget [1] --------
	Microsoft::WRL::ComPtr<ID3D11Texture2D> renderTargetBuffer2;
	D3D11_TEXTURE2D_DESC texture2dDesc2{};
	texture2dDesc2.Width = width;
	texture2dDesc2.Height = height;
	texture2dDesc2.MipLevels = 1;
	texture2dDesc2.ArraySize = 1;
	texture2dDesc2.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	texture2dDesc2.SampleDesc.Count = 1;
	texture2dDesc2.SampleDesc.Quality = 0;
	texture2dDesc2.Usage = D3D11_USAGE_DEFAULT;
	texture2dDesc2.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texture2dDesc2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texture2dDesc2.MiscFlags = 0;
	hr = gfx.device->CreateTexture2D(&texture2dDesc2, 0, renderTargetBuffer2.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc2{};
	renderTargetViewDesc2.Format = texture2dDesc.Format;
	renderTargetViewDesc2.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	hr = gfx.device->CreateRenderTargetView(renderTargetBuffer2.Get(), &renderTargetViewDesc2, renderTargetViews[1].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));


	//-------- ShaderResourceView [0] --------
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
	shaderResourceViewDesc.Format = texture2dDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;
	hr = gfx.device->CreateShaderResourceView(renderTargetBuffer.Get(), &shaderResourceViewDesc, shaderResourceViews[0].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	//-------- ShaderResourceView [1] --------
	hr = gfx.device->CreateShaderResourceView(renderTargetBuffer2.Get(), &shaderResourceViewDesc, shaderResourceViews[1].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	//-------- DepthStencil [0] --------
	texture2dDesc.CPUAccessFlags = 0;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer; 
	texture2dDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	hr = gfx.device->CreateTexture2D(&texture2dDesc, 0, depthStencilBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Flags = 0;
	hr = gfx.device->CreateDepthStencilView(depthStencilBuffer.Get(), &depthStencilViewDesc, depthStencilViews[0].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	//-------- DepthStencil [1] --------
	texture2dDesc.CPUAccessFlags = 0;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer1;
	texture2dDesc.Width = 2048;
	texture2dDesc.Height = 2048;
	texture2dDesc.MipLevels = 1;
	texture2dDesc.ArraySize = 1;
	texture2dDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texture2dDesc.SampleDesc.Count = 1;
	texture2dDesc.SampleDesc.Quality = 0;
	texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
	texture2dDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texture2dDesc.CPUAccessFlags = 0;
	texture2dDesc.MiscFlags = 0;
	hr = gfx.device->CreateTexture2D(&texture2dDesc, 0, depthStencilBuffer1.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc1{};
	depthStencilViewDesc1.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc1.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc1.Flags = 0;
	hr = gfx.device->CreateDepthStencilView(depthStencilBuffer1.Get(), &depthStencilViewDesc1, depthStencilViews[1].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	//-------- ShaderResourceView [2] --------
	shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	hr = gfx.device->CreateShaderResourceView(depthStencilBuffer.Get(), &shaderResourceViewDesc, shaderResourceViews[2].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	//-------- ShaderResourceView [3] --------
	shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	hr = gfx.device->CreateShaderResourceView(depthStencilBuffer1.Get(), &shaderResourceViewDesc, shaderResourceViews[3].GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));


	// ------- viewport -------
	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;



	////
	srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

}

void FrameBuffer::Clear(float r, float g, float b, float a, float depth)
{
	Graphics& gfx = Graphics::Instance();

	float color[4] = { r,g,b,a };
	for (int i = 0; i < 2; i++) {
		gfx.deviceContext->ClearRenderTargetView(renderTargetViews[i].Get(), color);
		gfx.deviceContext->ClearDepthStencilView(depthStencilViews[i].Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, depth, 0);
	}
	//gfx.deviceContext->ClearRenderTargetView(renderTargetViews->Get(), color);
}

void FrameBuffer::Activate()
{
	Graphics& gfx = Graphics::Instance();

	viewportCount = D3D10_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	gfx.deviceContext->RSGetViewports(&viewportCount, cachedViewports);
	gfx.deviceContext->OMGetRenderTargets(1, cachedRenderTargetView.ReleaseAndGetAddressOf(), cachedDepthStencilView.ReleaseAndGetAddressOf());
	gfx.deviceContext->RSSetViewports(1, &viewport);

	ID3D11RenderTargetView* rtv[2] = {
		renderTargetViews[0].Get(),
		renderTargetViews[1].Get(),
	};

	gfx.deviceContext->OMSetRenderTargets(2, rtv, depthStencilViews[0].Get());
}

void FrameBuffer::DeActivate()
{
	Graphics& gfx = Graphics::Instance();

	gfx.deviceContext->RSSetViewports(viewportCount, cachedViewports);
	gfx.deviceContext->OMSetRenderTargets(1, cachedRenderTargetView.GetAddressOf(), cachedDepthStencilView.Get());
}

void FrameBuffer::ShadowActivate()
{
	Graphics& gfx = Graphics::Instance();

	viewportCount = D3D10_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	gfx.deviceContext->RSGetViewports(&viewportCount, cachedViewports);
	gfx.deviceContext->OMGetRenderTargets(1, cachedRenderTargetView.ReleaseAndGetAddressOf(), cachedDepthStencilView.ReleaseAndGetAddressOf());
	gfx.deviceContext->RSSetViewports(1, &viewport);

	gfx.deviceContext->OMSetRenderTargets(0, nullptr, depthStencilViews[1].Get());
}

void FrameBuffer::ShadowDeActivate()
{
	Graphics& gfx = Graphics::Instance();

	gfx.deviceContext->RSSetViewports(viewportCount, cachedViewports);
	gfx.deviceContext->OMSetRenderTargets(1, cachedRenderTargetView.GetAddressOf(), cachedDepthStencilView.Get());
}

