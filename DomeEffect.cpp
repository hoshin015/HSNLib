#include "DomeEffect.h"
#include "Library/3D/ResourceManager.h"
#include "Library/Graphics/Graphics.h"
#include "Library/ErrorLogger.h"
#include "Library/Graphics/Shader.h"
#include "Library/Timer.h"
#include "Library/Input/InputManager.h"
#include "SpinningTopPlayerManager.h"

DomeEffect::DomeEffect(float radius)
{
	model = ResourceManager::Instance().LoadModelResource("Data/Fbx/cylinder01/cylinder01.fbx");

	//--- < ���_�V�F�[�_�[�I�u�W�F�N�g�Ɠ��̓��C�A�E�g�I�u�W�F�N�g�̐��� > ---
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

	//--- < �s�N�Z���V�F�[�_�[�I�u�W�F�N�g�̐��� > ---
	csoName = { "./Data/Shader/ParryEffect_PS.cso" };
	CreatePsFromCso(csoName, pixelShader.GetAddressOf());

	//--- �萔�o�b�t�@�I�u�W�F�N�g�̐��� ---
	D3D11_BUFFER_DESC constantBufferDesc{};
	constantBufferDesc.ByteWidth = sizeof(Constants);
	constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	HRESULT hr = Graphics::Instance().device->CreateBuffer(&constantBufferDesc, nullptr, constantBuffer.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	// �X�P�[���ύX
	SetScale({ nowScale,0.2,nowScale });

	// �T�C�Y�ݒ�
	maxScale = radius;
}

// �X�V����
void DomeEffect::Update()
{
	uvScrollValue.y -= uvScrollSpeed * Timer::Instance().DeltaTime();

	nowScale += 120.0f * Timer::Instance().DeltaTime();
	nowAlpha -= 2.0f * Timer::Instance().DeltaTime();
	if (nowScale > maxScale) nowScale = maxScale;
	if (nowAlpha < 0)
	{
		nowAlpha = 0;
	}

	// �X�P�[���ύX
	SetScale({ nowScale,0.4,nowScale });

	//InputManager& input = InputManager::Instance();
	//
	//if (input.GetKeyPressed(DirectX::Keyboard::Enter))
	//{
	//	StartEffect();
	//}

	// �s��X�V
	UpdateTransform();
}

// �`�揈��
void DomeEffect::Render()
{
	Graphics& gfx = Graphics::Instance();

	// --- < �萔�o�b�t�@�̍X�V�o�C���h > ---
	Constants data = { uvScrollValue };
	gfx.deviceContext->UpdateSubresource(constantBuffer.Get(), 0, 0, &data, 0, 0);
	gfx.deviceContext->VSSetConstantBuffers(7, 1, constantBuffer.GetAddressOf());

	// ���̃V�F�[�_�[��ۑ����Ă���
	gfx.deviceContext->VSGetShader(storeVertexShader.GetAddressOf(), nullptr, 0);
	gfx.deviceContext->PSGetShader(storePixelShader.GetAddressOf(), nullptr, 0);

	//--- < �V�F�[�_�[�̃o�C���h > ---
	gfx.deviceContext->VSSetShader(vertexShader.Get(), nullptr, 0);
	gfx.deviceContext->PSSetShader(pixelShader.Get(), nullptr, 0);

	gfx.SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_CULL_NONE);
	model->Render(transform, { color.x,color.y,color.z,nowAlpha }, NULL);

	// ���ɖ߂�
	gfx.SetRasterizer(RASTERIZER_STATE::CLOCK_TRUE_SOLID);
	gfx.deviceContext->VSSetShader(storeVertexShader.Get(), nullptr, 0);
	gfx.deviceContext->PSSetShader(storePixelShader.Get(), nullptr, 0);
}


// �G�t�F�N�g�J�n
void DomeEffect::StartEffect(float maxScale, DirectX::XMFLOAT3 color)
{
	nowAlpha = 3.0f;
	nowScale = 0.0f;

	this->maxScale = maxScale;
	this->color = color;
}
