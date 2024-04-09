#pragma once
#include <memory>
#include "Library/3D/SkinnedMesh.h"
#include "Stage.h"

// ステージ
class StageMain : public Stage
{
public:
	StageMain();
	~StageMain() override;

	// 更新処理
	void Update() override;

	// 描画処理
	void Render() override;

	// レイキャスト
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) override;

private:
	SkinnedMesh* model = nullptr;
	SkinnedMesh* collisionModel = nullptr;
};