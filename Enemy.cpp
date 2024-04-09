#include "Enemy.h"
#include "EnemyManager.h"
#include "Collision.h"

// レイキャスト
bool Enemy::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	return Collision::IntersectRayVsModel(start, end, model.get(), transform, hit);
}

// 破棄
void Enemy::Destroy()
{
	EnemyManager::Instance().Remove(this);
}
