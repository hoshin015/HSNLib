#pragma once
#include "Character.h"
#include "Library/3D/SkinnedMesh.h"
#include "Collision.h"

#include "BehaviorTreeIF.h"
#include "BehaviorTree.h"

// �G�l�~�[
class SpinningTopEnemy : public Character, public IBTree
{
public:
	enum class KIND
	{
		NONE = 0,
		ROOT,
		IDLE,
		WANDER,
	};


public:
	SpinningTopEnemy() {};
	~SpinningTopEnemy() override {}

	// �X�V����
	virtual void Update() = 0;

	// �`�揈��
	virtual void Render() = 0;

	// ���C�L���X�g
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);

	// �j��
	void Destroy();

public:
	// enemies �ł� index
	int id;

	// �v���C���[�Ƃ̋���
	float playerDistance;

	// behaviorTree
	std::unique_ptr<BTree> aiTree;
	bool GetBTreeJudge(const int kind) override;
	IBTree::STATE ActBTree(const int kind) override;
};