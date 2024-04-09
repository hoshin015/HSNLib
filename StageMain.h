#pragma once
#include <memory>
#include "Library/3D/SkinnedMesh.h"
#include "Stage.h"

// �X�e�[�W
class StageMain : public Stage
{
public:
	StageMain();
	~StageMain() override;

	// �X�V����
	void Update() override;

	// �`�揈��
	void Render() override;

	// ���C�L���X�g
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) override;

private:
	SkinnedMesh* model = nullptr;
	SkinnedMesh* collisionModel = nullptr;
};