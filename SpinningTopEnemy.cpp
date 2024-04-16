#include "SpinningTopEnemy.h"
#include "SpinningTopEnemyManager.h"
#include "Collision.h"

// レイキャスト
bool SpinningTopEnemy::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	return Collision::IntersectRayVsModel(start, end, model.get(), transform, hit);
}

// 破棄
void SpinningTopEnemy::Destroy()
{
	SpinningTopEnemyManager::Instance().Remove(this);
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
		ang.y+=2;
		SetAngle(ang);
		return  IBTree::STATE::Run;
		break;
	case KIND::IDLE:
		return  IBTree::STATE::Complete;
		break;
	case KIND::SEEK:
		// 回転
		ang = GetAngle();
		ang.y += rotationSpeed;
		SetAngle(ang);

		// ベクター作成
		DirectX::XMVECTOR POSITION = DirectX::XMLoadFloat3(&GetPosition());
		DirectX::XMVECTOR TARGET = DirectX::XMLoadFloat3(&targetPosition);
		DirectX::XMVECTOR VELOCITY = DirectX::XMLoadFloat3(&velocity);

		// ターゲットへのベクトルを求める
		DirectX::XMVECTOR ToTarget = DirectX::XMVectorSubtract(TARGET, POSITION);
		ToTarget = DirectX::XMVector3Normalize(ToTarget);				// 正規化
		ToTarget = DirectX::XMVectorScale(ToTarget, steeringMaxForce);	// スケーリング

		ToTarget = DirectX::XMVectorSubtract(ToTarget, VELOCITY);
		DirectX::XMVECTOR LENGTH = DirectX::XMVector3Length(ToTarget);
		float length;
		DirectX::XMStoreFloat(&length, LENGTH);

		if (length > steeringMaxSpeed)
		{
			ToTarget = DirectX::XMVector3Normalize(ToTarget);				// 正規化
			ToTarget = DirectX::XMVectorScale(ToTarget, steeringMaxSpeed);	// スケーリング
		}
		DirectX::XMStoreFloat3(&steeringForce, ToTarget);

		velocity.x += steeringForce.x;
		velocity.z += steeringForce.z;

		return  IBTree::STATE::Run;
		break;
	}

	return IBTree::STATE::Complete;
}
