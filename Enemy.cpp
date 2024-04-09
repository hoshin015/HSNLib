#include "Enemy.h"
#include "EnemyManager.h"
#include "Collision.h"

// ���C�L���X�g
bool Enemy::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	return Collision::IntersectRayVsModel(start, end, model.get(), transform, hit);
}

// �j��
void Enemy::Destroy()
{
	EnemyManager::Instance().Remove(this);
}
