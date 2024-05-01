#pragma once
#include "Library/3D/SkinnedMesh.h"
#include "Collision.h"

class Obstacle
{
public:
	Obstacle() {}
	virtual ~Obstacle() {};

	// �X�V����
	virtual void Update() {}

	// �`�揈��
	void Render();

	// �f�o�b�O�v���~�e�B�u�`��
	void DrawDebugPrimitive();


	// �ʒu�擾
	const DirectX::XMFLOAT3& GetPosition() const { return position; }
	// �ʒu�ݒ�
	void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }
	// ��]�擾
	const DirectX::XMFLOAT3& GetAngle() const { return angle; }
	// ��]�ݒ�
	void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }
	// �X�P�[���擾
	const DirectX::XMFLOAT3& GetScale() const { return scale; }
	// �X�P�[���ݒ�
	void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }

	// ���a
	float GetRadius() { return radius; }
	void SetRadius(float radius) { this->radius = radius; }

	// ��]�p�x
	float GeRotationSpeed() { return rotationSpeed; }
	void SetRotationSpeed(float rotationSpeed) { this->rotationSpeed = rotationSpeed; }

	// ��R
	float GetFriction() { return frictionPower; }
	void SetFriction(float friction) { this->frictionPower = friction; }

	// ���C�L���X�g
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);
protected:
	// �s��X�V�֐�
	void UpdateTransform();

public:
	std::shared_ptr<SkinnedMesh> model;

	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT3 angle = { 0,0,0 };
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	DirectX::XMFLOAT4X4 transform =
	{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};

	DirectX::XMFLOAT3 targetPosition = { -1,-1,-1 };
	float moveSpeed = 1.0f;

	DirectX::XMFLOAT3 velocity = { 0,0,0 };
	float frictionPower = 5.0f;

	float rotationSpeed = 0;	// 1�b�ɉ�]����p�x

	float radius = 1.1f;

	bool isCollision = true;

	int hitDamae = 0;	// �v���C���[�ƏՓ˂����Ƃ��ɗ^����_���[�W
};