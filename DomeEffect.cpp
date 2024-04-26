#include "DomeEffect.h"
#include "Library/3D/ResourceManager.h"
#include "Library/Graphics/Graphics.h"
#include "Library/ErrorLogger.h"
#include "Library/Graphics/Shader.h"
#include "Library/Timer.h"
#include "Library/Input/InputManager.h"

DomeEffect::DomeEffect()
{
	model = ResourceManager::Instance().LoadModelResource("Data/Fbx/dome/dome.fbx");

	//--- < 頂点シェーダーオブジェクトと入力レイアウトオブジェクトの生成 > ---
	const char* csoName{ "./Data/Shader/ParryEffect_VS.cso" };

	D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"BONES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
	};

	CreateVsFromCso(csoName, vertexShader.GetAddressOf(), inputLayout.GetAddressOf(), inputElementDesc, _countof(inputElementDesc));

	//--- < ピクセルシェーダーオブジェクトの生成 > ---
	csoName = { "./Data/Shader/ParryEffect_PS.cso" };
	CreatePsFromCso(csoName, pixelShader.GetAddressOf());

	//--- 定数バッファオブジェクトの生成 ---
	D3D11_BUFFER_DESC constantBufferDesc{};
	constantBufferDesc.ByteWidth = sizeof(Constants);
	constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	HRESULT hr = Graphics::Instance().device->CreateBuffer(&constantBufferDesc, nullptr, constantBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	// スケール変更
	SetScale({ nowScale,nowScale,nowScale });
}

// 更新処理
void DomeEffect::Update()
{
	uvScrollValue.y -= uvScrollSpeed * Timer::Instance().DeltaTime();

	nowScale += 200.0f * Timer::Instance().DeltaTime();
	nowAlpha -= 1.5f * Timer::Instance().DeltaTime();
	if (nowScale > 10) nowScale = 10;
	if (nowAlpha < 0) nowAlpha = 0;

	InputManager& input = InputManager::Instance();
	
	if (input.GetKeyPressed(DirectX::Keyboard::Enter))
	{
		nowAlpha = 1.0f;
		nowScale = 0.0f;
	}

	// 行列更新
	UpdateTransform();
}

// 描画処理
void DomeEffect::Render()
{
	Graphics& gfx = Graphics::Instance();

	// --- < 定数バッファの更新バインド > ---
	Constants data = { uvScrollValue };
	gfx.deviceContext->UpdateSubresource(constantBuffer.Get(), 0, 0, &data, 0, 0);
	gfx.deviceContext->VSSetConstantBuffers(7, 1, constantBuffer.GetAddressOf());

	//--- < シェーダーのバインド > ---
	gfx.deviceContext->VSSetShader(vertexShader.Get(), nullptr, 0);
	gfx.deviceContext->PSSetShader(pixelShader.Get(), nullptr, 0);


	//gfx.SetBlend(BLEND_STATE::ADD);
	gfx.SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_CULL_NONE);
	model->Render(transform, { 1,0.62,1,nowAlpha }, NULL);

	// 元に戻す
	gfx.SetBlend(BLEND_STATE::ALPHA);
	gfx.SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_CULL_NONE);
	gfx.deviceContext->VSSetShader(gfx.vertexShaders[static_cast<size_t>(VS_TYPE::SkinnedMesh_VS)].Get(), nullptr, 0);
	gfx.deviceContext->PSSetShader(gfx.pixelShaders[static_cast<size_t>(PS_TYPE::SkinnedMesh_PS)].Get(), nullptr, 0);
}
