#include "SpinningTopEnemy.h"
#include "EnemyManager.h"
#include "Collision.h"

// レイキャスト
bool SpinningTopEnemy::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	return Collision::IntersectRayVsModel(start, end, model.get(), transform, hit);
}

// 破棄
void SpinningTopEnemy::Destroy()
{
	//EnemyManager::Instance().Remove(this);
}

bool SpinningTopEnemy::GetBTreeJudge(const int _kind)
{
	KIND kind = (KIND)_kind;
	switch (kind)
	{
	case KIND::WANDER:
		return true;
	}

	return true;
}

IBTree::STATE SpinningTopEnemy::ActBTree(const int _kind)
{
	KIND kind = (KIND)_kind;
	switch (kind)
	{
	case KIND::WANDER:
		DirectX::XMFLOAT3 ang = GetAngle();
		ang.y++;
		SetAngle(ang);
		return  IBTree::STATE::Complete;
		break;
	case KIND::IDLE:
		return  IBTree::STATE::Complete;
		break;
	}

	return IBTree::STATE::Complete;
}
