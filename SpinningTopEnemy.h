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
		WANDER,
		SEEK,
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

public:
	// enemies での index
	int id;

	// プレイヤーとの距離
	float playerDistance;


	float rotationSpeed = 1;

	// ----- SteeringBehavior -----
	DirectX::XMFLOAT3 targetPosition = { 3,0,0 };

	float steeringMaxForce = 3.0f;
	float steeringMaxSpeed = 10.0f;
	DirectX::XMFLOAT3 steeringForce = { 0,0,0 };


	// ----- behaviorTree -----
	std::unique_ptr<BTree> aiTree;
	bool GetBTreeJudge(const int kind) override;
	IBTree::STATE ActBTree(const int kind) override;
};