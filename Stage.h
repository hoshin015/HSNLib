#pragma once

#include "Collision.h"

// �X�e�[�W
class Stage
{
public:
	Stage() {}
	virtual ~Stage() {}

	// �X�V����
	virtual void Update() = 0;

	// �`�揈��
	virtual void Render() = 0;

	// ���C�L���X�g
	virtual bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) = 0;
};