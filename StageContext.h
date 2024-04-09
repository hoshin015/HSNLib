#pragma once
#include <memory>
#include "Library/3D/SkinnedMesh.h"
#include "Stage.h"

// �X�e�[�W
class StageContext : public Stage
{
public:
	StageContext();
	~StageContext() override;

	// �X�V����
	void Update() override;

	// �`�揈��
	void Render();

	// ���C�L���X�g
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) override;

private:
	SkinnedMesh* model = nullptr;
	SkinnedMesh* collisionModel = nullptr;
};