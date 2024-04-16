#include "StEnemy01.h""
#include "Library/Graphics/Graphics.h"
#include "Library/MemoryLeak.h"
#include "Library/Timer.h"
#include "Library/3D/DebugPrimitive.h"
#include "Library/3D/ResourceManager.h"
#include "Library/3D/Camera.h"
#include "Library/Input/InputManager.h"
#include "StageManager.h"

StEnemy01::StEnemy01()
{
	model = ResourceManager::Instance().LoadModelResource("Data/Fbx/SpinningTopTest/SpinningTopTest.fbx");

	// �A�j���[�V�����ύX
	PlayAnimation(5, true);

	// BehaviorTree���\�z
	aiTree = std::make_unique <BTree>(this);
	aiTree->AddNode((int)KIND::NONE, (int)KIND::ROOT, 0, IBTree::RULE::Priority, this);

	aiTree->AddNode((int)KIND::ROOT, (int)KIND::ARRIVAL, 0, IBTree::RULE::Non, this);
	aiTree->AddNode((int)KIND::ROOT, (int)KIND::SEEK, 1, IBTree::RULE::Non, this);
	aiTree->AddNode((int)KIND::ROOT, (int)KIND::WANDER, 2, IBTree::RULE::Non, this);
	aiTree->AddNode((int)KIND::ROOT, (int)KIND::IDLE, 3, IBTree::RULE::Non, this);
}

StEnemy01::~StEnemy01()
{
}

void StEnemy01::Update()
{
	// ��]
	DirectX::XMFLOAT3 ang = GetAngle();
	ang.y += rotationSpeed;
	SetAngle(ang);

	UpdateTargetPosition();

	aiTree->Update();

	// ���͍X�V����
	UpdateVelocity();
	// ���G���ԍX�V
	UpdateInvincibleTimer();

	//UpdateAnimation();

	// �I�u�W�F�N�g�s��X�V
	UpdateTransform();
}

void StEnemy01::Render()
{
	model->Render(transform, { 1,1,1,1 }, &keyFrame);

	//DebugPrimitive::Instance().AddCylinder(position, radius, height, { 1,0,0,1 });

	DebugPrimitive::Instance().AddSphere(targetPosition, 0.2f, {0,1,0,1});
}

// TargetPosition �X�V
void StEnemy01::UpdateTargetPosition()
{
	// --- Graphics �擾 ---
	Graphics& gfx = Graphics::Instance();

	// �r���[�|�[�g
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	gfx.deviceContext->RSGetViewports(&numViewports, &viewport);

	// �ϊ��s��
	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&Camera::Instance().GetView());
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&Camera::Instance().GetProjection());
	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();
	// �G�l�~�[�z�u����
	InputManager& input = InputManager::Instance();
	if (input.GetMousePress(MOUSEBUTTON_STATE::leftButton))
	{
		// �}�E�X�J�[�\�����W���擾
		DirectX::XMFLOAT3 screenPosition;
		screenPosition.x = static_cast<int>(input.GetCursorPosX());
		screenPosition.y = static_cast<int>(input.GetCursorPosY());

		DirectX::XMVECTOR ScreenPosition, WorldPosition;

		// ���C�̎n�_���Z�o
		screenPosition.z = 0.0f;
		ScreenPosition = DirectX::XMLoadFloat3(&screenPosition);

		WorldPosition = DirectX::XMVector3Unproject(
			ScreenPosition,
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
		DirectX::XMFLOAT3 rayStart;
		DirectX::XMStoreFloat3(&rayStart, WorldPosition);

		// ���C�̏I�_���Z�o
		screenPosition.z = 1.0f;
		ScreenPosition = DirectX::XMLoadFloat3(&screenPosition);

		WorldPosition = DirectX::XMVector3Unproject(
			ScreenPosition,
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
		DirectX::XMFLOAT3 rayEnd;
		DirectX::XMStoreFloat3(&rayEnd, WorldPosition);

		// ���C�L���X�g
		HitResult hit;
		if (StageManager::Instance().RayCast(rayStart, rayEnd, hit))
		{
			// �G��z�u
			targetPosition = hit.position;
		}
	}
}

// ���S����
void StEnemy01::OnDead()
{
	Destroy();
}
