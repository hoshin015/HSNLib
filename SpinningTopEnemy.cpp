#include "SpinningTopEnemy.h"
#include "SpinningTopEnemyManager.h"
#include "Collision.h"
#include "Library/Timer.h"
#include "Library/3D/DebugPrimitive.h"
#include "Library/3D/LineRenderer.h"

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

// ----- SteeringBehavior -----
DirectX::XMFLOAT3 SpinningTopEnemy::SbSeek()
{
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

	DirectX::XMFLOAT3 steering;
	DirectX::XMStoreFloat3(&steering, STEERING);

	return steering;
}

DirectX::XMFLOAT3 SpinningTopEnemy::SbArrival()
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

	DirectX::XMFLOAT3 steering;
	DirectX::XMStoreFloat3(&steering, STEERING);

	return steering;
}

DirectX::XMFLOAT3 SpinningTopEnemy::SbWander()
{
	// 円の中心へのベクトルを算出
	DirectX::XMVECTOR VELOCITY = DirectX::XMLoadFloat3(&velocity);
	DirectX::XMVECTOR N_VELOCITY = DirectX::XMVector3Normalize(VELOCITY);
	DirectX::XMVECTOR CircleCenter = DirectX::XMVectorScale(N_VELOCITY, circleDistance);

	// 円の中心からのベクトルを算出
	float velAngle = atan2(velocity.x, velocity.z);
	DirectX::XMFLOAT3 fromCenter = { cosf(DirectX::XMConvertToRadians(wanderAngle)) + cosf(velAngle), 0, sinf(DirectX::XMConvertToRadians(wanderAngle)) + sinf(velAngle)};
	DirectX::XMVECTOR FromCenter = DirectX::XMLoadFloat3(&fromCenter);
	FromCenter = DirectX::XMVector3Normalize(FromCenter);
	FromCenter = DirectX::XMVectorScale(FromCenter, circleRadius);

	// ２つのベクトルを加算して移動ベクトルを計算
	DirectX::XMVECTOR STEERING = DirectX::XMVectorAdd(CircleCenter, FromCenter);

	DirectX::XMVECTOR LENGTH = DirectX::XMVector3LengthSq(STEERING);
	float length;
	DirectX::XMStoreFloat(&length, LENGTH);

	if (length > steeringMaxForce)
	{
		STEERING = DirectX::XMVector3Normalize(STEERING);				// 正規化
		STEERING = DirectX::XMVectorScale(STEERING, steeringMaxForce);	// スケーリング
	}

	DirectX::XMFLOAT3 steering;
	DirectX::XMStoreFloat3(&steering, STEERING);

	// 次の角度を決める
	{
		// 次の角度を決めるのは 1/60に一回
		wanderAngleChangeTimer += Timer::Instance().DeltaTime();
		if (wanderAngleChangeTimer > 1.0f / 60.0f)
		{
			float addAngle = ((rand() % (wanderAngleChange * 2)) - wanderAngleChange);
			wanderAngle += addAngle;
			while (wanderAngle >= 360) wanderAngle -= 360;
			while (wanderAngle < 0) wanderAngle += 360;

			wanderAngleChangeTimer -= 1.0f / 60.0f;
		}
	}

	
#if 1
	{
		// 球体描画
		DirectX::XMFLOAT3 circlePos;
		DirectX::XMStoreFloat3(&circlePos, CircleCenter);
		circlePos.x += position.x;
		circlePos.z += position.z;
		DebugPrimitive::Instance().AddSphere(circlePos, circleRadius, { 0,0,1,1 });

		// 線描画
		const DirectX::XMFLOAT4 white = DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1);
		const DirectX::XMFLOAT4 green = DirectX::XMFLOAT4(0, 1, 0, 1);
		const DirectX::XMFLOAT4 yellow = DirectX::XMFLOAT4(1, 1, 0, 1);
		LineRenderer& lineRenderer = LineRenderer::Instance();

		// 本体から球体への線 描画
		lineRenderer.AddVertex({ GetPosition().x, 0.2, GetPosition().z }, yellow);
		lineRenderer.AddVertex({ circlePos.x, 0.2, circlePos.z }, yellow);

		// 球体から球体の側面への線
		fromCenter;
		DirectX::XMStoreFloat3(&fromCenter, FromCenter);
		lineRenderer.AddVertex({ circlePos.x, 0.2, circlePos.z }, yellow);
		lineRenderer.AddVertex({ circlePos.x + fromCenter.x, 0.2, circlePos.z + fromCenter.z }, yellow);

		// 本体から球体の側面への線
		lineRenderer.AddVertex({ GetPosition().x, 0.2, GetPosition().z }, green);
		lineRenderer.AddVertex({ circlePos.x + fromCenter.x, 0.2, circlePos.z + fromCenter.z }, green);
	}
#endif	

	return steering;
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
	case KIND::IDLE:
		return  IBTree::STATE::Complete;
		break;
	case KIND::SEEK:
	{
		steeringForce = { 0,0,0 };
		DirectX::XMVECTOR SteeringForce = DirectX::XMLoadFloat3(&steeringForce);

		DirectX::XMFLOAT3 seekPower = SbSeek();
		DirectX::XMVECTOR SeekPower = DirectX::XMLoadFloat3(&seekPower);
		
		SteeringForce = DirectX::XMVectorAdd(SteeringForce, SeekPower);
		DirectX::XMStoreFloat3(&steeringForce, SteeringForce);

		velocity.x += steeringForce.x;
		velocity.z += steeringForce.z;

		Move(velocity.x, velocity.z, maxMoveSpeed);

		return  IBTree::STATE::Run;
		break;
	}
	case KIND::ARRIVAL:
	{
		steeringForce = { 0,0,0 };
		DirectX::XMVECTOR SteeringForce = DirectX::XMLoadFloat3(&steeringForce);

		DirectX::XMFLOAT3 arrivalPower = SbArrival();
		DirectX::XMVECTOR ArrivalPower = DirectX::XMLoadFloat3(&arrivalPower);

		SteeringForce = DirectX::XMVectorAdd(SteeringForce, ArrivalPower);
		DirectX::XMStoreFloat3(&steeringForce, SteeringForce);

		velocity.x += steeringForce.x;
		velocity.z += steeringForce.z;

		Move(velocity.x, velocity.z, maxMoveSpeed);

		return  IBTree::STATE::Run;

		break;
	}
	case KIND::WANDER:
		steeringForce = { 0,0,0 };
		DirectX::XMVECTOR SteeringForce = DirectX::XMLoadFloat3(&steeringForce);

		DirectX::XMFLOAT3 seekPower = SbSeek();
		DirectX::XMVECTOR SeekPower = DirectX::XMLoadFloat3(&seekPower);
		SeekPower = DirectX::XMVectorScale(SeekPower, 1.0f);
		SteeringForce = DirectX::XMVectorAdd(SteeringForce, SeekPower);

		DirectX::XMFLOAT3 wanderPower = SbWander();
		DirectX::XMVECTOR WanderPower = DirectX::XMLoadFloat3(&wanderPower);
		WanderPower = DirectX::XMVectorScale(WanderPower, 1.0f);
		
		SteeringForce = DirectX::XMVectorAdd(SteeringForce, WanderPower);
		DirectX::XMStoreFloat3(&steeringForce, SteeringForce);

		velocity.x += steeringForce.x;
		velocity.z += steeringForce.z;

		Move(velocity.x, velocity.z, maxMoveSpeed);

		return  IBTree::STATE::Run;
		break;
	}

	return IBTree::STATE::Complete;
}
