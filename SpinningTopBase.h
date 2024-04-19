#pragma once
#include <DirectXMath.h>
#include <memory>
#include "Library/3D/SkinnedMesh.h"
#include "Library/Effekseer/Effect.h"

class SpinningTopBase
{
public:
	SpinningTopBase() {}
	virtual ~SpinningTopBase() {}

	// �s��X�V�֐�
	void UpdateTransform();

	// �s��擾
	const DirectX::XMFLOAT4X4& GetTransform() const { return transform; }

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
	// ���a�ݒ�
	void SetRadius(const float radius) { this->radius = radius; }

	// velocity�擾
	DirectX::XMFLOAT3 GetVelocity() { return this->velocity; }
	// velocity�ݒ�
	void SetVelocity(const DirectX::XMFLOAT3& velocity) { this->velocity = velocity; }

	// �ڒn����擾
	bool IsGround() const { return isGround; }

	// ���a�擾
	float GetRadius() const { return radius; }
	// �����擾
	float GetHeight() const { return height; }

	// ���N��Ԃ��擾
	int GetHealth() const { return health; }
	// �ő匒�N��Ԃ��擾
	int GetMaxHealth() const { return maxHealth; }



	// �_���[�W��^����
	bool ApplyDamage(int damage, float invincibleTime);
	// �Ռ���^����
	void AddImpulse(const DirectX::XMFLOAT3& impulse);

protected:
	// �ړ�����
	void Move(float vx, float vz, float speed);
	// ���񏈗�
	void Turn(float vx, float vz, float speed);
	// �W�����v����
	void Jump(float speed);
	// ���͍X�V����
	void UpdateVelocity();

	// ���n����
	virtual void OnLanding() {}

	// �_���[�W����
	virtual void OnDamaged() {}
	// ���S����
	virtual void OnDead() {}

	// ���G���ԏ���
	void UpdateInvincibleTimer();



private:
	// �������͍X�V����
	void UpdateVerticalVelocity();
	// �����ړ��X�V����
	void UpdateVerticalMove();
	// �������͍X�V����
	void UpdateHorizontalVelocity();
	// �����ړ��X�V����
	void UpdateHorizontalMove();

protected:
	//--------------------------------------------------------------
	//  �G�t�F�N�g
	//--------------------------------------------------------------

	// �G�t�F�N�g�X�V����
	void UpdateEffect();
	// �G�t�F�N�g�t���O�N���A
	void ClearEffectFlag();

	//--------------------------------------------------------------
	//  ���ʉ�
	//--------------------------------------------------------------

	// ���ʉ��X�V����
	void UpdateSE();
	// ���ʉ��t���O�N���A
	void ClearSEFlag();

	//--------------------------------------------------------------
	//  �A�j���[�V����
	//--------------------------------------------------------------

	// �A�j���[�V�����Đ�
	void PlayAnimation(int index, bool loop);
	// �A�j���[�V�����X�V
	void UpdateAnimation();

	//--------------------------------------------------------------
	//  �f�o�b�O�`��
	//--------------------------------------------------------------
	void DrawDebugPrimitive();

protected:
	DirectX::XMFLOAT3		position = { 0,0,0 };
	DirectX::XMFLOAT3		angle = { 0,0,0 };
	DirectX::XMFLOAT3		scale = { 1,1,1 };
	DirectX::XMFLOAT4X4		transform =
	{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
	DirectX::XMFLOAT3 velocity = { 0,0,0 };
	float gravity = -1.0f;
	bool isGround = false;

	float radius = 0.5f;
	float height = 1.0f;

	int health = 5;
	int maxHealth = 5;
	float invincibleTimer = 1.0f;

	float friction = 0.25f;
	float airControl = 0.3f;

	float acceleration = 1.0f;
	float maxMoveSpeed = 15.0f;
	float moveVecX = 0.0f;
	float moveVecZ = 0.0f;

	// RayCast�̃I�t�Z�b�g
	float stepOffset = 0.5f;
	float stepOffset1 = 0.1f;
	float slopeRate = 1.0f;

public:
	std::shared_ptr<SkinnedMesh>	model;
	std::shared_ptr<SkinnedMesh>	paryEffect;

	// --- �A�j���[�V�����t���O ---
	Animation::KeyFrame keyFrame;			// �g�p�����L�[�t���[��
	int currentKeyFrame = 0;
	int currentAnimationIndex = 0;			// ���ݍĐ����̃A�j���[�V�����ԍ�
	float currentAnimationSeconds = 0.0f;	// ���ݍĐ����̕b��

	bool animationLoopFlag = true;			// ���[�v�Đ����邩�ǂ���
	bool animationEndFlag = false;			// �A�j���[�V�������Ō�̃L�[�t���[���ɓ��B�������ǂ���

	float animationBlendTime = 0.0f;
	float animationBlendSeconds = 0.0f;
	int blendAnimationIndex = -1;			// �J�ڑO�̃A�j���[�V�����ԍ�
};