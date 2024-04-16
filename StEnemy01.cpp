#include "StEnemy01.h""
#include "Library/Graphics/Graphics.h"
#include "Library/MemoryLeak.h"
#include "Library/Timer.h"
#include "Library/3D/DebugPrimitive.h"
#include "Library/3D/ResourceManager.h"

StEnemy01::StEnemy01()
{
	model = ResourceManager::Instance().LoadModelResource("Data/Fbx/Jummo/Jummo.fbx");

	// アニメーション変更
	PlayAnimation(5, true);

	// BehaviorTreeを構築
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

	// 速力更新処理
	UpdateVelocity();
	// 無敵時間更新
	UpdateInvincibleTimer();

	UpdateAnimation();

	// オブジェクト行列更新
	UpdateTransform();
}

void StEnemy01::Render()
{
	model->Render(transform, { 1,1,1,1 }, &keyFrame);

	DebugPrimitive::Instance().AddCylinder(position, radius, height, { 1,0,0,1 });
}

// 死亡処理
void StEnemy01::OnDead()
{
	Destroy();
}
