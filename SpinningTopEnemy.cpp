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
		// ベクター作成
		DirectX::XMVECTOR POSITION = DirectX::XMLoadFloat3(&GetPosition());
		DirectX::XMVECTOR TARGET = DirectX::XMLoadFloat3(&targetPosition);
		DirectX::XMVECTOR VELOCITY = DirectX::XMLoadFloat3(&velocity);
		DirectX::XMVECTOR DSERIRED_VELOCITY;
		DirectX::XMVECTOR STEERING;

		// ターゲットへのベクトルを求める
		DirectX::XMVECTOR ToTarget = DirectX::XMVectorSubtract(TARGET, POSITION);
		ToTarget = DirectX::XMVector3Normalize(ToTarget);					// 正規化
		DSERIRED_VELOCITY = DirectX::XMVectorScale(ToTarget, maxMoveSpeed);	// スケーリング

		// ステアリング力の計算
		STEERING = DirectX::XMVectorSubtract(DSERIRED_VELOCITY, VELOCITY);
		DirectX::XMVECTOR LENGTH = DirectX::XMVector3LengthSq(STEERING);
		float length;
		DirectX::XMStoreFloat(&length, LENGTH);

		if (length > steeringMaxForce)
		{
			STEERING = DirectX::XMVector3Normalize(STEERING);				// 正規化
			STEERING = DirectX::XMVectorScale(STEERING, steeringMaxForce);	// スケーリング
		}

		DirectX::XMStoreFloat3(&steeringForce, STEERING);

		velocity.x += steeringForce.x;
		velocity.z += steeringForce.z;

		Move(velocity.x, velocity.z, maxMoveSpeed);

		return  IBTree::STATE::Run;
		break;
	case KIND::ARRIVAL:
	{
		// ベクター作成
		DirectX::XMVECTOR POSITION = DirectX::XMLoadFloat3(&GetPosition());
		DirectX::XMVECTOR TARGET = DirectX::XMLoadFloat3(&targetPosition);
		DirectX::XMVECTOR VELOCITY = DirectX::XMLoadFloat3(&velocity);
		DirectX::XMVECTOR DSERIRED_VELOCITY;
		DirectX::XMVECTOR STEERING;

		// ターゲットへのベクトルを求める
		DirectX::XMVECTOR ToTarget = DirectX::XMVectorSubtract(TARGET, POSITION);
		DirectX::XMVECTOR TargetDistance = DirectX::XMVector3Length(ToTarget);
		float targetDistance;
		DirectX::XMStoreFloat(&targetDistance, TargetDistance);

		// slow エリア
		if (targetDistance > slowingArea)
		{
			ToTarget = DirectX::XMVector3Normalize(ToTarget);					// 正規化
			DSERIRED_VELOCITY = DirectX::XMVectorScale(ToTarget, maxMoveSpeed);	// スケーリング
		}
		else
		{
			ToTarget = DirectX::XMVector3Normalize(ToTarget);					// 正規化
			DSERIRED_VELOCITY = DirectX::XMVectorScale(ToTarget, maxMoveSpeed);	// スケーリング
			DSERIRED_VELOCITY = DirectX::XMVectorScale(DSERIRED_VELOCITY, targetDistance / slowingArea);
		}
		// ステアリング力の計算
		STEERING = DirectX::XMVectorSubtract(DSERIRED_VELOCITY, VELOCITY);
		DirectX::XMVECTOR LENGTH = DirectX::XMVector3LengthSq(STEERING);
		float length;
		DirectX::XMStoreFloat(&length, LENGTH);

		if (length > steeringMaxForce)
		{
			STEERING = DirectX::XMVector3Normalize(STEERING);				// 正規化
			STEERING = DirectX::XMVectorScale(STEERING, steeringMaxForce);	// スケーリング
		}

		DirectX::XMStoreFloat3(&steeringForce, STEERING);

		velocity.x += steeringForce.x;
		velocity.z += steeringForce.z;

		Move(velocity.x, velocity.z, maxMoveSpeed);

		return  IBTree::STATE::Run;

		break;
	}
	}

	return IBTree::STATE::Complete;
}
