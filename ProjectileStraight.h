#pragma once

#include "Library/3D/SkinnedMesh.h"
#include "Projectile.h"
#include "ProjectileManager.h"

// ���i����
class ProjectileStraight : public Projectile
{
public:
	ProjectileStraight(ProjectileManager* manager);
	~ProjectileStraight() override;

	// �X�V����
	void Update();

	// �`�揈��
	void Render() override;

	// ����
	void Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position);

private:
	SkinnedMesh* model = nullptr;
	float speed = 5.0f;
	float lifeTimer = 3.0f;
	float radius = 0.4f;
};