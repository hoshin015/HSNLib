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
	};


public:
	SpinningTopEnemy() {};
	~SpinningTopEnemy() override {}

	// 更新処理
	virtual void Update() = 0;

	// 描画処理
	virtual void Render() = 0;

	// レイキャスト
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);

	// 破棄
	void Destroy();

	// ----- SteeringBehavior -----
	DirectX::XMFLOAT3 SbSeek();
	DirectX::XMFLOAT3 SbArrival();
	DirectX::XMFLOAT3 SbWander();

public:
	// enemies での index
	int id;

	// プレイヤーとの距離
	float playerDistance;


	float rotationSpeed = 1;

	// ----- SteeringBehavior -----
	DirectX::XMFLOAT3 targetPosition = { 3,0,0 };

	float steeringMaxForce = 1.1f;
	float slowingArea = 3.0f;
	float circleDistance = 4.0f;
	float circleRadius = 2.0f;
	float wanderAngle = 0;
	int wanderAngleChange = 30;
	float wanderAngleChangeTimer = 0.0f;

	DirectX::XMFLOAT3 steeringForce = { 0,0,0 };


	// ----- behaviorTree -----
	std::unique_ptr<BTree> aiTree;
	bool GetBTreeJudge(const int kind) override;
	IBTree::STATE ActBTree(const int kind) override;
};