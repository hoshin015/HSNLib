#pragma once

#include <d3d11.h>
#include <wrl.h>
#include <cstdint>
class BloomBuffer
{
public:
	BloomBuffer(uint32_t width, uint32_t height);
	virtual ~BloomBuffer() = default;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> allRTV;						// 全てを書きこむrtv
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> noLuminanceExtractRTV;		// 高輝度抽出の際に除外するrtv

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceViews[3];

	D3D11_VIEWPORT viewport;

	void Clear(float r = 0, float g = 0, float b = 0, float a = 1, float depth = 1);
	void Activate();
	void DeActivate();

private:
	UINT viewportCount = D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE;
	D3D11_VIEWPORT cachedViewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE];
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> cachedRenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> cachedDepthStencilView;


	Microsoft::WRL::ComPtr<ID3D11Texture2D> renderTargetTexture;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
};

