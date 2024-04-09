#include "EnemyDragon.h"
#include "Library/Graphics/Graphics.h"
#include "Library/MemoryLeak.h"
#include "Library/Timer.h"
#include "Library/3D/DebugPrimitive.h"
#include "Library/3D/ResourceManager.h"

EnemyDragon::EnemyDragon()
{
	model = ResourceManager::Instance().LoadModelResource("Data/Fbx/Albino/Albino.fbx");

	SetRadius(1.5f);

	// �A�j���[�V�����ύX
	PlayAnimation(6, true);
}

EnemyDragon::~EnemyDragon()
{
}

void EnemyDragon::Update()
{
	// ���͍X�V����
	UpdateVelocity();
	// ���G���ԍX�V
	UpdateInvincibleTimer();

	UpdateAnimation();

	UpdateEffect();

	// �I�u�W�F�N�g�s��X�V
	UpdateTransform();
}

void EnemyDragon::Render()
{
	using namespace DirectX;

	// --- Graphics �擾 ---
	Graphics& gfx = Graphics::Instance();

	// -- rasterizer �̐ݒ� ---
	gfx.SetRasterizer(RASTERIZER_STATE::CLOCK_TRUE_SOLID);

	// --- ���f���`�� ---
	model->Render(transform, { 1,1,1,1 }, &keyFrame);

	// --- �����蔻��`�� ---
	int sphereCollisionCount = model->skeletonSpheres.size();
	for (int i = 0; i < sphereCollisionCount; i++)
	{
		float radius = model->skeletonSpheres.at(i).radius;

		DirectX::XMFLOAT3 collisionPosition = { 0,0,0 };


		if (model->skeletonSpheres.at(i).name != "")
		{
			const Animation::KeyFrame& keyframe = model->animationClips.at(currentAnimationIndex).sequence.at(currentKeyFrame);

			int nodeCount = keyframe.nodes.size();
			for (int nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
			{
				const Animation::KeyFrame::Node& node = keyframe.nodes.at(nodeIndex);

				// �{�[���̃m�[�h�Ɠ������O�Ȃ珈������
				if (node.name != model->skeletonSpheres.at(i).name) continue;

				DirectX::XMMATRIX T = DirectX::XMLoadFloat4x4(&transform);
				DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&node.globalTransform);

				DirectX::XMMATRIX W = M * T;
				DirectX::XMFLOAT4X4 w;
				DirectX::XMStoreFloat4x4(&w, W);

				collisionPosition = { w._41, w._42, w._43};
			}
		}

		DebugPrimitive::Instance().AddSphere(collisionPosition, radius, {0,0,1,1});
	}
	DebugPrimitive::Instance().AddCylinder(position, radius, height, { 1,0,0,1 });
}

// ���S����
void EnemyDragon::OnDead()
{
	Destroy();
}
