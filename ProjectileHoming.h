#pragma once

#include "Library/3D/SkinnedMesh.h"
#include "Projectile.h"
#include "ProjectileManager.h"

// ’Ç]’eŠÛ
class ProjectileHoming : public Projectile
{
public:
	ProjectileHoming(ProjectileManager* manager);
	~ProjectileHoming() override;

	// XVˆ—
	void Update() override;

	// •`‰æˆ—
	void Render() override;

	// ”­Ë
	void Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3 target);

private:
	SkinnedMesh* model = nullptr;
	DirectX::XMFLOAT3 target = { 0,0,0 };
	float moveSpeed = 5.0f;
	float turnSpeed = DirectX::XMConvertToRadians(180);
	float lifeTimer = 3.0f;
};