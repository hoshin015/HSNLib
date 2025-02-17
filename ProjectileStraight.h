#pragma once

#include "Library/3D/SkinnedMesh.h"
#include "Projectile.h"
#include "ProjectileManager.h"

// 直進処理
class ProjectileStraight : public Projectile
{
public:
	ProjectileStraight(ProjectileManager* manager);
	~ProjectileStraight() override;

	// 更新処理
	void Update();

	// 描画処理
	void Render() override;

	// 発射
	void Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position);

private:
	SkinnedMesh* model = nullptr;
	float speed = 5.0f;
	float lifeTimer = 3.0f;
	float radius = 0.4f;
};