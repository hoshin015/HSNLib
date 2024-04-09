#include "EnemySlime.h"
#include "Library/Graphics/Graphics.h"
#include "Library/MemoryLeak.h"
#include "Library/Timer.h"
#include "Library/3D/DebugPrimitive.h"
#include "Library/3D/ResourceManager.h"

EnemySlime::EnemySlime()
{
	model = ResourceManager::Instance().LoadModelResource("Data/Fbx/Jummo/Jummo.fbx");

	// �A�j���[�V�����ύX
	//PlayAnimation(6, true);
	PlayAnimation(5, true);
}

EnemySlime::~EnemySlime()
{
}

void EnemySlime::Update()
{
	// ���͍X�V����
	UpdateVelocity();
	// ���G���ԍX�V
	UpdateInvincibleTimer();

	UpdateAnimation();

	// �I�u�W�F�N�g�s��X�V
	UpdateTransform();
}

void EnemySlime::Render()
{
	using namespace DirectX;

	// --- Graphics �擾 ---
	Graphics& gfx = Graphics::Instance();

	// rasterizer �̐ݒ�
	gfx.SetRasterizer(RASTERIZER_STATE::CLOCK_TRUE_SOLID);

	model->Render(transform, { 1,1,1,1 }, &keyFrame);

	DebugPrimitive::Instance().AddCylinder(position, radius, height, { 1,0,0,1 });
}

// ���S����
void EnemySlime::OnDead()
{
	Destroy();
}
