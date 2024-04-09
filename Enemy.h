#pragma once

#include "Character.h"
#include "Library/3D/SkinnedMesh.h"
#include "Collision.h"

// �G�l�~�[
class Enemy : public Character
{
public:
	Enemy() {}
	~Enemy() override {}

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
};