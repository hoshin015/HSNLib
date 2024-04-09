#pragma once

#include "Library/3D/SkinnedMesh.h"
#include "Projectile.h"
#include "ProjectileManager.h"

// ’¼iˆ—
class ProjectileStraight : public Projectile
{
public:
	ProjectileStraight(ProjectileManager* manager);
	~ProjectileStraight() override;

	// XVˆ—
	void Update();

	// •`‰æˆ—
	void Render() override;

	// ”­Ë
	void Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position);

private:
	SkinnedMesh* model = nullptr;
	float speed = 5.0f;
	float lifeTimer = 3.0f;
	float radius = 0.4f;
};