#include "DamageText.h"
#include "DamageTextManager.h"
#include "Library/Text/DispString.h"
#include "Library/Graphics/Graphics.h"
#include "Library/3D/Camera.h"
#include "Library/Timer.h"

// �X�V����
void DamageText::Update()
{
	switch (state)
	{
	case DamageText::State::Idle:
		UpdateIdleState();
		break;
	case DamageText::State::fadeOut:
		UpdateFadeOutState();
		break;
	}

	// ���W�X�V
	UpdatePosition();
}

// �`�揈��
void DamageText::Render()
{
	float fontSize = 40;
	DamageTextManager::Instance().textSpr->SprTextOut(strText, { drawPosition.x - fontSize*0.5f, drawPosition.y - fontSize*0.5f }, { fontSize, fontSize });
	//DispString::Instance().Draw(text, { drawPosition.x, drawPosition.y }, size, TEXT_ALIGN::MIDDLE, color, false, { 0,0,0,1 }, 2);
}

// �j��
void DamageText::Destroy()
{
	DamageTextManager::Instance().Remove(this);
}

// ���W�X�V
void DamageText::UpdatePosition()
{
	// ---- �|�W�V�����X�V ---

	// Graphics �擾 
	Graphics& gfx = Graphics::Instance();

	// �r���[�|�[�g
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	gfx.deviceContext->RSGetViewports(&numViewports, &viewport);

	// �ϊ��s��
	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&Camera::Instance().GetView());
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&Camera::Instance().GetProjection());
	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

	// �G�l�~�[����̍��W�擾
	DirectX::XMFLOAT3 worldPosition = position;
	worldPosition.y += addY;
	DirectX::XMVECTOR WorldPosition = DirectX::XMLoadFloat3(&worldPosition);

	// ���[���h���W����X�N���[�����W�ւ̕ϊ�
	DirectX::XMVECTOR ScreenPosition = DirectX::XMVector3Project(
		WorldPosition,
		viewport.TopLeftX,
		viewport.TopLeftY,
		viewport.Width,
		viewport.Height,
		viewport.MinDepth,
		viewport.MaxDepth,
		Projection,
		View,
		World
	);

	// �X�N���[�����W
	DirectX::XMFLOAT2 screenPosition;
	DirectX::XMStoreFloat2(&drawPosition, ScreenPosition);
}


//==========================================================================
//
//�@�X�e�[�g����
// 
//==========================================================================

// �ҋ@�X�e�[�g�֑J��
void DamageText::TransitionIdleState()
{
	timer = 0.0f;

	state = State::Idle;
}

// �ҋ@�X�e�[�g�X�V����
void DamageText::UpdateIdleState()
{
	// �^�C�}�[����
	timer += Timer::Instance().DeltaTime();
	if (timer >= idleTime)
	{
		TransitionFadeOutState();
		return;
	}
}

// �t�F�[�h�A�E�g�X�e�[�g�֑J��
void DamageText::TransitionFadeOutState()
{
	timer = 0.0f;

	state = State::fadeOut;
}

// �t�F�[�h�A�E�g�X�e�[�g�X�V����
void DamageText::UpdateFadeOutState()
{
	// �^�C�}�[����
	timer += Timer::Instance().DeltaTime();
	if (timer >= fadeOutTime)
	{
		Destroy();
		return;
	}

	addY += 4 * Timer::Instance().DeltaTime();
}
