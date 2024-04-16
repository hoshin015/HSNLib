#include "StEnemy01.h""
#include "Library/Graphics/Graphics.h"
#include "Library/MemoryLeak.h"
#include "Library/Timer.h"
#include "Library/3D/DebugPrimitive.h"
#include "Library/3D/ResourceManager.h"

StEnemy01::StEnemy01()
{
	model = ResourceManager::Instance().LoadModelResource("Data/Fbx/Jummo/Jummo.fbx");

	// �A�j���[�V�����ύX
	PlayAnimation(5, true);

	// BehaviorTree���\�z
	aiTree = std::make_unique <BTree>(this);
	aiTree->AddNode((int)KIND::NONE, (int)KIND::ROOT, 0, IBTree::RULE::Priority, this);

	aiTree->AddNode((int)KIND::ROOT, (int)KIND::WANDER, 0, IBTree::RULE::Non, this);
	aiTree->AddNode((int)KIND::ROOT, (int)KIND::IDLE, 1, IBTree::RULE::Non, this);
}

StEnemy01::~StEnemy01()
{
}

void StEnemy01::Update()
{
	aiTree->Update();

	// ���͍X�V����
	UpdateVelocity();
	// ���G���ԍX�V
	UpdateInvincibleTimer();

	UpdateAnimation();

	// �I�u�W�F�N�g�s��X�V
	UpdateTransform();
}

void StEnemy01::Render()
{
	model->Render(transform, { 1,1,1,1 }, &keyFrame);

	DebugPrimitive::Instance().AddCylinder(position, radius, height, { 1,0,0,1 });
}

// ���S����
void StEnemy01::OnDead()
{
	Destroy();
}
