#pragma once

#include "Character.h"
#include "ProjectileManager.h"

enum class PlayerAnim
{
	Attack01,
	Attack02,
	Attack03,
	IdleBattle,
	JumpAirNormal,
	MoveFwdBattle,
	Victory
};

// �v���C���[
class Player : public Character
{
public:
	Player();
	~Player() override;

	// �X�V����
	void Update();

	// �`�揈��
	void Render();

	// �f�o�b�O�pGUI�`��
	void DrawDebugGui();

private:
	
	// �ړ����͏���
	bool InputMove();
	// �X�e�B�b�N���͒l����ړ��x�N�g�����擾
	DirectX::XMFLOAT3 GetMoveVec() const;
	// �W�����v���͏���
	bool InputJump();
	// �U�����͏���
	bool InputAttack();

	// ���n����
	void OnLanding() override;

	// �e�ۓ��͏���
	void InputProjectile();
	
	//--------------------------------------------------------------
	//  �Փ˔���
	//--------------------------------------------------------------
	
	// �v���C���[�ƃG�l�~�[�̂��炢����̏Փ�
	void CollisionBodyPlayerVsEnemies();

	// �v���C���[�ƃG�l�~�[�̏Փˏ���
	void CollisionPlayerVsEnemies();
	// �e�ۂƓG�̏Փˏ���
	void CollisionProjectilesVsEnemies();
	// �A�j���[�V�����ƃG�l�~�[�̏Փˏ���
	void CollisionNodeVsEnemies();

	
	
	//--------------------------------------------------------------
	//  �X�e�[�g�Ǘ�
	//--------------------------------------------------------------

	// �ҋ@�X�e�[�g�֑J��
	void TransitionIdleState();
	// �ҋ@�X�e�[�g�X�V����
	void UpdateIdleState();

	// �ړ��X�e�[�g�֑J��
	void TransitionMoveState();
	// �ړ��X�e�[�g�X�V����
	void UpdateMoveState();

	// �W�����v�X�e�[�g�֑J��
	void TransitionJumpState();
	// �W�����v�X�e�[�g�X�V����
	void UpdateJumpState();

	// �U���X�e�[�g�֑J��
	void TransitionAttackState();
	// �U���X�e�[�g�X�V����
	void UpdateAttackState();

	// �����X�e�[�g�֑J��
	void TransitionVictoryState();
	// �����X�e�[�g�X�V����
	void UpdateVictoryUpdate();

private:

	ProjectileManager projectileManager;

	float moveSpeed = 5.0f;
	float turnSpeed = 720;
	float jumpSpeed = 20.0f;
	int jumpCount = 0;
	int jumpLimit = 2;

	float radius = 0.4f;
	float height = 1.4f;
	

	// --- �X�e�[�g�Ǘ� ---
	enum class State
	{
		Idle,
		Move,
		Jump,
		Land,
		Attack,
		Victory,
	};
	State state = State::Idle;
};