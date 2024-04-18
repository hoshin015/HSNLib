#pragma once
#include "SpinningTopBase.h"
#include "Library/3D/SkinnedMesh.h"
#include "Collision.h"

#include "BehaviorTreeIF.h"
#include "BehaviorTree.h"

// エネミー
class SpinningTopEnemy : public SpinningTopBase, public IBTree
{
public:
	enum class KIND
	{
		NONE = 0,
		ROOT,
		IDLE,
		SEEK,
		ARRIVAL,
		WANDER,
		CollisionAvoidance,
		SeekPlayer,
		WanderSpawnArea,
	};

public:
	SpinningTopEnemy() {};
	~SpinningTopEnemy() override {}

	// 更新処理
	virtual void Update() = 0;

	// 描画処理
	virtual void Render() = 0;

	// デバッグプリミティブ描画
	virtual void DrawDebugPrimitive() {};

	// レイキャスト
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);

	// 破棄
	void Destroy();

	// ----- SteeringBehavior -----
	DirectX::XMFLOAT3 SbSeek();
	DirectX::XMFLOAT3 SbArrival();
	DirectX::XMFLOAT3 SbWander();
	DirectX::XMFLOAT3 SbCollisionAvoidance();

public:
	// enemies での index
	int id;

	// プレイヤーとの距離
	float playerDistance;
	DirectX::XMFLOAT3 plPosition = { 0,0,0 };	// TODO: デバッグ用だから削除する

	
	float rotationSpeed = 720;	// 1秒に回転する角度

	float searchRadius = 6.0f;		// プレイヤー追従開始の範囲
	float notSearchRadius = 15.0f;	// プレイヤーの追従をあきらめる範囲


	// ----- SteeringBehavior -----
	DirectX::XMFLOAT3 steeringForce = { 0,0,0 };
	DirectX::XMFLOAT3 targetPosition = { 3,0,0 };
	DirectX::XMFLOAT3 spawnPosition = { 0,0,0 };

	float steeringMaxForce = 1.0f;
	float slowingArea = 1.0f;
	float circleDistance = 3.0f;
	float circleRadius = 2.5f;
	float wanderAngle = 0;
	int wanderAngleChange = 30;
	float wanderAngleChangeTimer = 0.0f;
	float wanderAngleChangeTime = 0.1f;

	float maxSeeAhead = 6.0f;
	float maxAvoidForce = 60.0f;

	// ----- behaviorTree -----
	std::unique_ptr<BTree> aiTree;
	bool GetBTreeJudge(const int kind) override;
	IBTree::STATE ActBTree(const int kind) override;
};