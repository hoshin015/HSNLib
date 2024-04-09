#pragma once

#include "Enemy.h"

enum class EnemyContextBaseSlimeAnim
{
	Attack01,
	Attack02,
	Die,
	Dizzy,
	GetHit,
	IdleBattle,
	IdleNormal,
	RunFWD,
	SenseSomethingPRT,
	SemseSomethingST,
	Taunt,
	Victory,
	WalkBWD,
	WalkFWD,
	WalkLeft,
	WalkRight
};


class EnemyContextBaseSlime : public Enemy
{
public:
	EnemyContextBaseSlime();
	~EnemyContextBaseSlime() override;

	// �X�V����
	void Update() override;

	// �`�揈��
	void Render() override;

	// �f�o�b�O�pGUI�`��
	void DrawDebugGui();

	// ContextBase �`��
	void DrawContext();


protected:
	// ���S����
	void OnDead() override;

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

	//--------------------------------------------------------------
	//  ContextBase
	//--------------------------------------------------------------

	// �v���C���[�ւ̃��C�L���X�g
	void RayTarget();

	// �p�x�v�Z
	void CalcOneRadius();
	// �����`�F�b�N
	void InterestCheck();
	// �댯�`�F�b�N
	void AccelerationCheck();
	// �ŏI�I�Ȉړ��x�N�g���Z�o
	void CalcContextVec();

	// �ǐՏI���`�F�b�N
	void EndChaseCheck();


private:
	float radius = 0.5f;
	float height = 1.0f;


	// --- �X�e�[�g�Ǘ� ---
	enum class State
	{
		Idle,
		Move,
		Attack,
		Hit,
		DIe
	};
	State state = State::Idle;

	// --- ContextBase ---

	static const int numRays = 12;		// ���C�̖{��
	float lookAhead = 5;				// ���C�̋���

	float rayDirections[numRays] = {};	// ���C�̊p�x
	float interest[numRays] = {};		// �Ή�����p�x�ɂǂꂾ���ړ���������
	float danger[numRays] = {};			// �Ή�����p�x�ɂǂꂾ���ړ��������Ȃ���
		
	float targetMinDistance = 2.0f;		// ���̋����܂ł����߂Â��Ȃ�
	float targetMaxDistance = 5.0f;		// ���̋�������͏��X�Ɏg�Â������ӗ~����

	DirectX::XMFLOAT3 contextVelocity = { 0,0,0 };		// �ŏI�I�Ɉړ�����x�N�g��
		
	DirectX::XMFLOAT3 lastTargetPosition = { 0,0,0 };	// �ۑ����Ă����^�[�Q�b�g�̈ʒu
};