#include "EnemySlime.h"
#include "Library/Graphics/Graphics.h"
#include "Library/MemoryLeak.h"
#include "Library/Timer.h"
#include "Library/3D/DebugPrimitive.h"
#include "Library/3D/ResourceManager.h"

EnemySlime::EnemySlime()
{
	model = ResourceManager::Instance().LoadModelResource("Data/Fbx/Jummo/Jummo.fbx");

	// アニメーション変更
	//PlayAnimation(6, true);
	PlayAnimation(5, true);
}

EnemySlime::~EnemySlime()
{
}

void EnemySlime::Update()
{
	// 速力更新処理
	UpdateVelocity();
	// 無敵時間更新
	UpdateInvincibleTimer();

	UpdateAnimation();

	// オブジェクト行列更新
	UpdateTransform();
}

void EnemySlime::Render()
{
	using namespace DirectX;

	// --- Graphics 取得 ---
	Graphics& gfx = Graphics::Instance();

	// rasterizer の設定
	gfx.SetRasterizer(RASTERIZER_STATE::CLOCK_TRUE_SOLID);

	model->Render(transform, { 1,1,1,1 }, &keyFrame);

	DebugPrimitive::Instance().AddCylinder(position, radius, height, { 1,0,0,1 });
}

// 死亡処理
void EnemySlime::OnDead()
{
	Destroy();
}
