#include "SpinningTopEnemy.h"
#include "SpinningTopEnemyManager.h"
#include "Collision.h"
#include "Library/Timer.h"
#include "Library/3D/DebugPrimitive.h"
#include "Library/3D/LineRenderer.h"
#include "Library/ImGui/ConsoleData.h"
#include "PlayerManager.h"

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
	STEERING = DirectX::XMVectorScale(STEERING, Timer::Instance().DeltaTime());
	DirectX::XMVECTOR LENGTH = DirectX::XMVector3Length(STEERING);
	float length;
	DirectX::XMStoreFloat(&length, LENGTH);

	if (length > steeringMaxForce)
	{
		STEERING = DirectX::XMVector3Normalize(STEERING);				// 正規化
		STEERING = DirectX::XMVectorScale(STEERING, steeringMaxForce);	// スケーリング
	}

	DirectX::XMFLOAT3 steering;
	DirectX::XMStoreFloat3(&steering, STEERING);

#if 0
	{
		// 線描画
		const DirectX::XMFLOAT4 white = DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1);
		const DirectX::XMFLOAT4 red = DirectX::XMFLOAT4(1, 0, 0, 1);
		const DirectX::XMFLOAT4 green = DirectX::XMFLOAT4(0, 1, 0, 1);
		const DirectX::XMFLOAT4 yellow = DirectX::XMFLOAT4(1, 1, 0, 1);
		LineRenderer& lineRenderer = LineRenderer::Instance();

		DirectX::XMFLOAT3 dseriredV;
		DirectX::XMStoreFloat3(&dseriredV, DSERIRED_VELOCITY);

		// ターゲットへのベクトル
		lineRenderer.AddVertex({ GetPosition().x, 0.2, GetPosition().z }, red);
		lineRenderer.AddVertex({ GetPosition().x + dseriredV.x, 0.2, GetPosition().z + dseriredV.z }, red);

		// velocityベクトル
		lineRenderer.AddVertex({ GetPosition().x, 0.2, GetPosition().z }, green);
		lineRenderer.AddVertex({ GetPosition().x + velocity.x, 0.2, GetPosition().z + velocity.z }, green);


		lineRenderer.AddVertex({ GetPosition().x + velocity.x, 0.2, GetPosition().z + velocity.z }, yellow);
		lineRenderer.AddVertex({ GetPosition().x + velocity.x + steering.x, 0.2, GetPosition().z + velocity.x + steering.z }, yellow);
	}
#endif	

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
	STEERING = DirectX::XMVectorScale(STEERING, Timer::Instance().DeltaTime());
	DirectX::XMVECTOR LENGTH = DirectX::XMVector3Length(STEERING);

	float length;
	DirectX::XMStoreFloat(&length, LENGTH);

	if (length > steeringMaxForce)
	{
		STEERING = DirectX::XMVector3Normalize(STEERING);				// 正規化
		STEERING = DirectX::XMVectorScale(STEERING, steeringMaxForce);	// スケーリング
	}

	DirectX::XMFLOAT3 steering;
	DirectX::XMStoreFloat3(&steering, STEERING);

#if 1
	{
		// 線描画
		const DirectX::XMFLOAT4 white = DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1);
		const DirectX::XMFLOAT4 red = DirectX::XMFLOAT4(1, 0,0, 1);
		const DirectX::XMFLOAT4 green = DirectX::XMFLOAT4(0, 1, 0, 1);
		const DirectX::XMFLOAT4 yellow = DirectX::XMFLOAT4(1, 1, 0, 1);
		LineRenderer& lineRenderer = LineRenderer::Instance();

		DirectX::XMFLOAT3 dseriredV;
		DirectX::XMStoreFloat3(&dseriredV, DSERIRED_VELOCITY);
		
		// ターゲットへのベクトル
		lineRenderer.AddVertex({ GetPosition().x, 0.2, GetPosition().z }, red);
		lineRenderer.AddVertex({ GetPosition().x + dseriredV.x, 0.2, GetPosition().z + dseriredV.z }, red);

		// velocityベクトル
		lineRenderer.AddVertex({ GetPosition().x, 0.2, GetPosition().z }, green);
		lineRenderer.AddVertex({ GetPosition().x + velocity.x, 0.2, GetPosition().z + velocity.z }, green);


		lineRenderer.AddVertex({ GetPosition().x + velocity.x, 0.2, GetPosition().z + velocity.z }, yellow);
		lineRenderer.AddVertex({ GetPosition().x + velocity.x + steering.x, 0.2, GetPosition().z + velocity.x + steering.z }, yellow);
	}
#endif	

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
	STEERING = DirectX::XMVectorScale(STEERING, Timer::Instance().DeltaTime());

	DirectX::XMVECTOR LENGTH = DirectX::XMVector3Length(STEERING);
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
		wanderAngleChangeTimer += Timer::Instance().DeltaTime();
		if (wanderAngleChangeTimer > wanderAngleChangeTime)
		{
			float addAngle = ((rand() % (wanderAngleChange * 2)) - wanderAngleChange);
			wanderAngle += addAngle;
			while (wanderAngle >= 360) wanderAngle -= 360;
			while (wanderAngle < 0) wanderAngle += 360;


			Timer::Instance().CheckTick();
			float time = Timer::Instance().CheckTime();
			ConsoleData::Instance().logs.push_back("[ " + std::to_string(time) + " ] " + std::to_string(addAngle));

			wanderAngleChangeTimer -= wanderAngleChangeTime;
		}
	}

#if 1
	{
		// 球体描画
		DirectX::XMFLOAT3 circlePos;
		DirectX::XMStoreFloat3(&circlePos, CircleCenter);
		circlePos.x += position.x;
		circlePos.z += position.z;
		DebugPrimitive::Instance().AddCylinder(circlePos, circleRadius, 0.2, { 0,1,0,1 });

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
	case KIND::SeekPlayer:
		// プレイヤーが searchRadius の範囲内なら追跡する
		//DirectX::XMFLOAT3 plPosition = PlayerManager::Instance().GetPlayer(0)->GetPosition();
		DirectX::XMFLOAT3 playerPosition = this->plPosition;
		DirectX::XMVECTOR PlPOSITION = DirectX::XMLoadFloat3(&playerPosition);
		DirectX::XMVECTOR POSITION = DirectX::XMLoadFloat3(&GetPosition());
		DirectX::XMVECTOR LENGTH = DirectX::XMVector3Length(DirectX::XMVectorSubtract(PlPOSITION, POSITION));
		float length;
		DirectX::XMStoreFloat(&length, LENGTH);
		if (length < searchRadius)
		{
			return true;
		}
		// 範囲内にいないなら除外
		return false;
	case KIND::WanderSpawnArea:
		
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

		//Move(velocity.x, velocity.z, maxMoveSpeed);

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

		//Move(velocity.x, velocity.z, maxMoveSpeed);

		return  IBTree::STATE::Run;

		break;
	}
	case KIND::WANDER:
		steeringForce = { 0,0,0 };
		DirectX::XMVECTOR SteeringForce = DirectX::XMLoadFloat3(&steeringForce);

		DirectX::XMFLOAT3 seekPower = SbSeek();
		DirectX::XMVECTOR SeekPower = DirectX::XMLoadFloat3(&seekPower);
		SeekPower = DirectX::XMVectorScale(SeekPower, 0.5f);
		SteeringForce = DirectX::XMVectorAdd(SteeringForce, SeekPower);

		DirectX::XMFLOAT3 wanderPower = SbWander();
		DirectX::XMVECTOR WanderPower = DirectX::XMLoadFloat3(&wanderPower);
		WanderPower = DirectX::XMVectorScale(WanderPower, 0.9f);
		
		SteeringForce = DirectX::XMVectorAdd(SteeringForce, WanderPower);
		DirectX::XMStoreFloat3(&steeringForce, SteeringForce);

		velocity.x += steeringForce.x;
		velocity.z += steeringForce.z;

		//Move(velocity.x, velocity.z, maxMoveSpeed);

		return  IBTree::STATE::Run;
		break;
	case KIND::SeekPlayer:
	{
		// プレイヤーが追従可能範囲外にでたらノードを抜ける
		{
			DirectX::XMFLOAT3 playerPosition = this->plPosition;
			DirectX::XMVECTOR PlPOSITION = DirectX::XMLoadFloat3(&playerPosition);
			DirectX::XMVECTOR POSITION = DirectX::XMLoadFloat3(&GetPosition());
			DirectX::XMVECTOR LENGTH = DirectX::XMVector3Length(DirectX::XMVectorSubtract(PlPOSITION, POSITION));
			float length;
			DirectX::XMStoreFloat(&length, LENGTH);
			if (length > notSearchRadius)
			{
				return  IBTree::STATE::Complete;
			}
		}

		steeringForce = { 0,0,0 };
		DirectX::XMVECTOR SteeringForce = DirectX::XMLoadFloat3(&steeringForce);

		targetPosition = plPosition;
		DirectX::XMFLOAT3 seekPower = SbSeek();
		DirectX::XMVECTOR SeekPower = DirectX::XMLoadFloat3(&seekPower);

		SteeringForce = DirectX::XMVectorAdd(SteeringForce, SeekPower);
		DirectX::XMStoreFloat3(&steeringForce, SteeringForce);

		velocity.x += steeringForce.x;
		velocity.z += steeringForce.z;

		return IBTree::STATE::Run;
		break;
	}
	case KIND::WanderSpawnArea:
	{
		steeringForce = { 0,0,0 };
		DirectX::XMVECTOR SteeringForce = DirectX::XMLoadFloat3(&steeringForce);

		targetPosition = spawnPosition;
		DirectX::XMFLOAT3 seekPower = SbSeek();
		DirectX::XMVECTOR SeekPower = DirectX::XMLoadFloat3(&seekPower);
		SeekPower = DirectX::XMVectorScale(SeekPower, 0.5f);
		SteeringForce = DirectX::XMVectorAdd(SteeringForce, SeekPower);

		DirectX::XMFLOAT3 wanderPower = SbWander();
		DirectX::XMVECTOR WanderPower = DirectX::XMLoadFloat3(&wanderPower);
		WanderPower = DirectX::XMVectorScale(WanderPower, 0.9f);

		SteeringForce = DirectX::XMVectorAdd(SteeringForce, WanderPower);
		DirectX::XMStoreFloat3(&steeringForce, SteeringForce);

		velocity.x += steeringForce.x;
		velocity.z += steeringForce.z;

		break;
	}
	
	}
	

	return IBTree::STATE::Complete;
}
