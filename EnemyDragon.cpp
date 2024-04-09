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

	// アニメーション変更
	PlayAnimation(6, true);
}

EnemyDragon::~EnemyDragon()
{
}

void EnemyDragon::Update()
{
	// 速力更新処理
	UpdateVelocity();
	// 無敵時間更新
	UpdateInvincibleTimer();

	UpdateAnimation();

	UpdateEffect();

	// オブジェクト行列更新
	UpdateTransform();
}

void EnemyDragon::Render()
{
	using namespace DirectX;

	// --- Graphics 取得 ---
	Graphics& gfx = Graphics::Instance();

	// -- rasterizer の設定 ---
	gfx.SetRasterizer(RASTERIZER_STATE::CLOCK_TRUE_SOLID);

	// --- モデル描画 ---
	model->Render(transform, { 1,1,1,1 }, &keyFrame);

	// --- 当たり判定描画 ---
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

				// ボーンのノードと同じ名前なら処理する
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

// 死亡処理
void EnemyDragon::OnDead()
{
	Destroy();
}
