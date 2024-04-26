#include "SpinningTopEnemy.h"
#include "SpinningTopEnemyManager.h"
#include "Collision.h"
#include "Library/Timer.h"
#include "Library/3D/DebugPrimitive.h"
#include "Library/3D/LineRenderer.h"
#include "Library/ImGui/ConsoleData.h"
#include "PlayerManager.h"
#include "ObstacleManager.h"

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

#if 0
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


			//Timer::Instance().CheckTick();
			//float time = Timer::Instance().CheckTime();
			//ConsoleData::Instance().logs.push_back("[ " + std::to_string(time) + " ] " + std::to_string(addAngle));

			wanderAngleChangeTimer -= wanderAngleChangeTime;
		}
	}

#if 0
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

DirectX::XMFLOAT3 SpinningTopEnemy::SbCollisionAvoidance()
{

	DirectX::XMFLOAT3 steering = { 0,0,0 };

	// ベクター作成
	DirectX::XMVECTOR POSITION = DirectX::XMLoadFloat3(&GetPosition());
	DirectX::XMVECTOR VELOCITY = DirectX::XMLoadFloat3(&velocity);
	DirectX::XMVECTOR N_VEC = DirectX::XMVector3Normalize(VELOCITY);

	DirectX::XMVECTOR L_VEC = VELOCITY;
	DirectX::XMVECTOR L_VEC_START;
	DirectX::XMVECTOR L_VEC_END;
	L_VEC = DirectX::XMVectorSetX(L_VEC, -velocity.z);
	L_VEC = DirectX::XMVectorSetZ(L_VEC, velocity.x);
	L_VEC = DirectX::XMVector3Normalize(L_VEC);
	L_VEC = DirectX::XMVectorScale(L_VEC, radius);
	DirectX::XMVECTOR R_VEC = VELOCITY;
	DirectX::XMVECTOR R_VEC_START;
	DirectX::XMVECTOR R_VEC_END;
	R_VEC = DirectX::XMVectorSetX(R_VEC, velocity.z);
	R_VEC = DirectX::XMVectorSetZ(R_VEC, -velocity.x);
	R_VEC = DirectX::XMVector3Normalize(R_VEC);
	R_VEC = DirectX::XMVectorScale(R_VEC, radius);

	DirectX::XMVECTOR AvoidanceForce;
	DirectX::XMVECTOR STEERING;
	DirectX::XMVECTOR ObsPos;
	DirectX::XMVECTOR PosToObs;
	DirectX::XMVECTOR HitPos;


	// 障害物と衝突しているかチェック
	ObstacleManager& obsM = ObstacleManager::Instance();

	// 一番近い障害物を探す
	int obsCount = obsM.GetObstacleCount();
	if (obsCount == 0) return {};
	Obstacle* obs = obsM.GetObstacle(0);
	float obsDistance = FLT_MAX;

	for (int i = 0; i < obsCount; i++)
	{
		Obstacle* iObs = obsM.GetObstacle(i);

		// obsのisCollisionがfalseなら無視
		if (!iObs->isCollision) continue;

		ObsPos = DirectX::XMLoadFloat3(&iObs->GetPosition());
		// 長さ取得
		PosToObs = DirectX::XMVectorSubtract(POSITION, ObsPos);
		DirectX::XMVECTOR LENGTH = DirectX::XMVector3Length(PosToObs);
		float length;
		DirectX::XMStoreFloat(&length, LENGTH);

		length -= iObs->radius;

		if (obsDistance > length)
		{
			obsDistance = length;
			obs = obsM.GetObstacle(i);
		}
	}
	ObsPos = DirectX::XMLoadFloat3(&obs->GetPosition());

	// ahead velocity を求める
	DirectX::XMVECTOR AHEAD = DirectX::XMVectorScale(N_VEC, maxSeeAhead);
	AHEAD = DirectX::XMVectorAdd(AHEAD, POSITION);
	
	L_VEC_START = DirectX::XMVectorAdd(POSITION, L_VEC);
	R_VEC_START = DirectX::XMVectorAdd(POSITION, R_VEC);
	L_VEC_END = DirectX::XMVectorAdd(AHEAD, L_VEC);
	R_VEC_END = DirectX::XMVectorAdd(AHEAD, R_VEC);

	DirectX::XMFLOAT3 rVecStart;
	DirectX::XMFLOAT3 lVecStart;
	DirectX::XMFLOAT3 rVecEnd;
	DirectX::XMFLOAT3 lVecEnd;
	DirectX::XMFLOAT3 ahead1;
	DirectX::XMFLOAT3 ahead2;
	DirectX::XMStoreFloat3(&rVecStart, R_VEC_START);
	DirectX::XMStoreFloat3(&lVecStart, L_VEC_START);
	DirectX::XMStoreFloat3(&rVecEnd, R_VEC_END);
	DirectX::XMStoreFloat3(&lVecEnd, L_VEC_END);
	rVecStart.y = 0.2f;
	lVecStart.y = 0.2f;
	rVecEnd.y = 0.2f;
	lVecEnd.y = 0.2f;

#if 0
	// 線描画
	const DirectX::XMFLOAT4 white = DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1);
	const DirectX::XMFLOAT4 green = DirectX::XMFLOAT4(0, 1, 0, 1);
	const DirectX::XMFLOAT4 red = DirectX::XMFLOAT4(1, 0, 0, 1);
	LineRenderer& lineRenderer = LineRenderer::Instance();

	lineRenderer.AddVertex({ lVecStart.x, 0.3, lVecStart.z }, white);
	lineRenderer.AddVertex({ lVecEnd.x, 0.3, lVecEnd.z }, white);

	lineRenderer.AddVertex({ rVecStart.x, 0.3, rVecStart.z }, white);
	lineRenderer.AddVertex({ rVecEnd.x , 0.3, rVecEnd.z }, white);
#endif
	float rLength = 0;
	DirectX::XMVECTOR RLength;
	float lLength = 0;
	DirectX::XMVECTOR LLength;

	HitResult hit;
	if (obs->RayCast(lVecStart, lVecEnd, hit))
	{
		// hitポジション
		HitPos = DirectX::XMLoadFloat3(&hit.position);
		// 長さ取得
		DirectX::XMVECTOR EndToObs = DirectX::XMVectorSubtract(L_VEC_END, ObsPos);
		LLength = DirectX::XMVector3Length(EndToObs);
		DirectX::XMStoreFloat(&lLength, LLength);
		
		DirectX::XMVECTOR HitToObs = DirectX::XMVectorSubtract(HitPos, ObsPos);
		AvoidanceForce = DirectX::XMVector3Normalize(HitToObs);
		AvoidanceForce = DirectX::XMVectorScale(AvoidanceForce, maxAvoidForce);

		//DirectX::XMFLOAT3 avoid;
		//DirectX::XMStoreFloat3(&avoid, AvoidanceForce);
		//lineRenderer.AddVertex({ hit.position.x, 0.3, hit.position.z }, green);
		//lineRenderer.AddVertex({ hit.position.x + avoid.x , 0.3, hit.position.z + avoid.z }, red);

		float power = 1.5 - (hit.distance / maxSeeAhead);

		STEERING = AvoidanceForce;
		STEERING = DirectX::XMVectorScale(STEERING, Timer::Instance().DeltaTime() * power);
		DirectX::XMStoreFloat3(&steering, STEERING);
	}

	if (obs->RayCast(rVecStart, rVecEnd, hit))
	{
		// obsポジション
		HitPos = DirectX::XMLoadFloat3(&hit.position);
		// 長さ取得
		DirectX::XMVECTOR EndToObs = DirectX::XMVectorSubtract(R_VEC_END, ObsPos);
		RLength = DirectX::XMVector3Length(EndToObs);
		DirectX::XMStoreFloat(&rLength, RLength);

		if (lLength < rLength)
		{
			DirectX::XMVECTOR HitToObs = DirectX::XMVectorSubtract(HitPos, ObsPos);
			AvoidanceForce = DirectX::XMVector3Normalize(HitToObs);
			AvoidanceForce = DirectX::XMVectorScale(AvoidanceForce, maxAvoidForce);

			float power = 1.5 - (hit.distance / maxSeeAhead);

			STEERING = AvoidanceForce;
			STEERING = DirectX::XMVectorScale(STEERING, Timer::Instance().DeltaTime() * power);
			DirectX::XMStoreFloat3(&steering, STEERING);
		}
	}

	return steering;
}

bool SpinningTopEnemy::GetBTreeJudge(const int _kind)
{
	KIND kind = (KIND)_kind;
	switch (kind)
	{
	case KIND::Generate:
		return (isGenerateFinish) ? false : true;
	case KIND::Down:
		return (isDown) ? true : false;
	case KIND::PlayerPursuit:
	{
		// クールタイムチェック
		chargeAttackCoolTimer += Timer::Instance().DeltaTime();
		if (chargeAttackCoolTimer < chargeAttackCoolTime)
		{
			return false;
		}


		// プレイヤーが pursuitRadius の範囲内なら追跡する
		//DirectX::XMFLOAT3 plPosition = PlayerManager::Instance().GetPlayer(0)->GetPosition();
		DirectX::XMFLOAT3 playerPosition = this->plPosition;
		DirectX::XMVECTOR PlPOSITION = DirectX::XMLoadFloat3(&playerPosition);
		DirectX::XMVECTOR POSITION = DirectX::XMLoadFloat3(&GetPosition());
		DirectX::XMVECTOR LENGTH = DirectX::XMVector3Length(DirectX::XMVectorSubtract(PlPOSITION, POSITION));
		float length;
		DirectX::XMStoreFloat(&length, LENGTH);
		if (length < pursuitRadius)
		{
			// クールタイムリセット
			chargeAttackCoolTimer = 0.0f;

			return true;
		}
		// 範囲内にいないなら除外
		return false;
	}
	case KIND::SeekPlayer:
	{
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
	}
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
	case KIND::Down:
	{
		downTimer += Timer::Instance().DeltaTime();
		if (downTimer > downTime)
		{
			downTimer = 0.0f;
			return  IBTree::STATE::Complete;
		}

		// 減速処理
		DirectX::XMVECTOR VELOCITY = DirectX::XMLoadFloat3(&velocity);
		DirectX::XMVECTOR VELOCITY_LENGTH = DirectX::XMVector3Length(VELOCITY);
		float velocityLength;
		DirectX::XMStoreFloat(&velocityLength, VELOCITY_LENGTH);

		if (velocityLength > 0.0f)
		{
			DirectX::XMVECTOR POSITION = DirectX::XMLoadFloat3(&position);
			POSITION = DirectX::XMVectorAdd(POSITION, DirectX::XMVectorScale(VELOCITY, Timer::Instance().DeltaTime()));
			DirectX::XMVECTOR SUB_VECLOCITY = DirectX::XMVectorScale(VELOCITY, downFrictionPower * Timer::Instance().DeltaTime());
			VELOCITY = DirectX::XMVectorSubtract(VELOCITY, SUB_VECLOCITY);												// 加速度の減少処理
			VELOCITY_LENGTH = DirectX::XMVector3Length(VELOCITY);
			DirectX::XMStoreFloat(&velocityLength, VELOCITY_LENGTH);
			if (velocityLength < 0.0f)
			{
				VELOCITY = DirectX::XMVectorZero();
			}

			DirectX::XMStoreFloat3(&velocity, VELOCITY);
			DirectX::XMStoreFloat3(&position, POSITION);
		}

		return IBTree::STATE::Run;
		break;
	}
	case KIND::CollisionAvoidance:
	{

		break;
	}
	case KIND::Generate:
	{
		if (isGround)
			isGenerateFinish = true;

		//velocity.x += 5;
		break;
	}
	case KIND::PlayerPositionGet:
	{
		// プレイヤーの座標を保存しておく
		targetPosition = this->plPosition;	//TODO: デバッグ用(本環境ではplayerの座標)

		// 停止処理
		velocity = { 0,0,0 };

		break;
	}
	case KIND::WaitChargeAttack:
	{
		// 減速処理
		//DirectX::XMVECTOR VLEOCITY = DirectX::XMLoadFloat3(&velocity);
		//DirectX::XMVectorScale(VLEOCITY, 0.01f);
		//DirectX::XMStoreFloat3(&velocity, VLEOCITY);

		waitChargeAttackTimer += Timer::Instance().DeltaTime();

		if (waitChargeAttackTimer > waitChargeAttackTime)
		{
			waitChargeAttackTimer = 0.0f;
			return  IBTree::STATE::Complete;
		}

		return  IBTree::STATE::Run;

		break;
	}
	case KIND::ChargeAttack:
	{
		// ターゲット座標の +-0.01 に到達したらノードを抜ける
		{
			DirectX::XMVECTOR TARGETPOSITION = DirectX::XMLoadFloat3(&targetPosition);
			DirectX::XMVECTOR POSITION = DirectX::XMLoadFloat3(&GetPosition());
			DirectX::XMVECTOR LENGTH = DirectX::XMVector3Length(DirectX::XMVectorSubtract(TARGETPOSITION, POSITION));
			float length;
			DirectX::XMStoreFloat(&length, LENGTH);
			if (length < 0.1)
			{
				return  IBTree::STATE::Complete;
			}
		}

		steeringForce = { 0,0,0 };
		DirectX::XMVECTOR SteeringForce = DirectX::XMLoadFloat3(&steeringForce);

		DirectX::XMFLOAT3 seekPower = SbSeek();
		DirectX::XMVECTOR SeekPower = DirectX::XMLoadFloat3(&seekPower);
		SeekPower = DirectX::XMVectorScale(SeekPower, 5.0f);
		SteeringForce = DirectX::XMVectorAdd(SteeringForce, SeekPower);

		DirectX::XMFLOAT3 avoidancePower = SbCollisionAvoidance();
		DirectX::XMVECTOR AvoidancePower = DirectX::XMLoadFloat3(&avoidancePower);
		AvoidancePower = DirectX::XMVectorScale(AvoidancePower, 1.0f);
		SteeringForce = DirectX::XMVectorAdd(SteeringForce, AvoidancePower);

		DirectX::XMStoreFloat3(&steeringForce, SteeringForce);

		velocity.x += steeringForce.x;
		velocity.z += steeringForce.z;

		// 何か問題があってスタックした場合の抜けるよう処理
		chargeAttackTimer += Timer::Instance().DeltaTime();
		if (chargeAttackTimer > chargeAttackTime)
		{
			chargeAttackTimer = 0.0f;
			return  IBTree::STATE::Complete;
		}

		return IBTree::STATE::Run;

		break;
	}
	case KIND::SeekPlayer:
	{
		steeringForce = { 0,0,0 };
		DirectX::XMVECTOR SteeringForce = DirectX::XMLoadFloat3(&steeringForce);

		targetPosition = plPosition;
		DirectX::XMFLOAT3 seekPower = SbSeek();
		DirectX::XMVECTOR SeekPower = DirectX::XMLoadFloat3(&seekPower);
		SeekPower = DirectX::XMVectorScale(SeekPower, 1.0f);
		SteeringForce = DirectX::XMVectorAdd(SteeringForce, SeekPower);

		DirectX::XMFLOAT3 avoidancePower = SbCollisionAvoidance();
		DirectX::XMVECTOR AvoidancePower = DirectX::XMLoadFloat3(&avoidancePower);
		AvoidancePower = DirectX::XMVectorScale(AvoidancePower, 5.0f);
		SteeringForce = DirectX::XMVectorAdd(SteeringForce, AvoidancePower);

		DirectX::XMStoreFloat3(&steeringForce, SteeringForce);

		velocity.x += steeringForce.x;
		velocity.z += steeringForce.z;

		// targetPosition の更新後にチェックしないと挙動がかわる
		{
			DirectX::XMFLOAT3 playerPosition = this->plPosition;
			DirectX::XMVECTOR PlPOSITION = DirectX::XMLoadFloat3(&playerPosition);
			DirectX::XMVECTOR POSITION = DirectX::XMLoadFloat3(&GetPosition());
			DirectX::XMVECTOR LENGTH = DirectX::XMVector3Length(DirectX::XMVectorSubtract(PlPOSITION, POSITION));
			float length;
			DirectX::XMStoreFloat(&length, LENGTH);

			// プレイヤーが攻撃範囲内ならノードを抜ける
			if (length < pursuitRadius)
			{
				return  IBTree::STATE::Complete;
			}

			// プレイヤーが追従可能範囲外にでたらノードを抜ける
			if (length > notSearchRadius)
			{
				return  IBTree::STATE::Complete;
			}
		}

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

		DirectX::XMFLOAT3 avoidancePower = SbCollisionAvoidance();
		DirectX::XMVECTOR AvoidancePower = DirectX::XMLoadFloat3(&avoidancePower);
		AvoidancePower = DirectX::XMVectorScale(AvoidancePower, 0.5f);
		
		SteeringForce = DirectX::XMVectorAdd(SteeringForce, AvoidancePower);

		DirectX::XMStoreFloat3(&steeringForce, SteeringForce);

		velocity.x += steeringForce.x;
		velocity.z += steeringForce.z;

		break;
	}
	}
	

	return IBTree::STATE::Complete;
}
