#pragma once
#include <DirectXMath.h>

enum ENEMY_KIND
{
	ENEMY_0,
	ENEMY_1,
	ENEMY_2,
	ENEMY_3,

	ENEMY_NUM,
};

enum BEHAVIOR_TYPE
{
	pursuit,
	chase,
};

enum ENEMY_SPAWN_EREA
{
	EREA0,
	EREA1,
	EREA2,
	EREA3,
	EREA4,

	EREA_NUM,
};

struct EnemyData
{
	int enemyKind;					// �G�̎��
	int behaviorType;				// �r�w�C�r�A�̎��

	float radius;					// �G�̔��a
	float maxMoveSpeed;				// �ő�ړ����x
	float maxHealth;				// �ő�HP

	float pursuitRadius;			// pursuit�͈�
	float searchRadius;				// ���G�͈�
	float notSearchRadius;			// ����G�͈�

	float circleDistance;			// wander�~�Ƃ̋���
	float circleRadius;				// wander�~�̔��a
	int wanderAngleChange;			// wander�~�̕ύX�p�x
	float wanderAngleChangeTime;	// wander�~�̕ύX�b��

	float chargeAttackCoolTime;		// chargeAttack�̃N�[���^�C���b��

	float waitChargeAttackTime;		// chargeAttack�̑҂�����
};

extern EnemyData enemyData[ENEMY_NUM];

struct EnemySpawnErea
{
	DirectX::XMFLOAT3 position;
	float radius = 3;
};

extern EnemySpawnErea enemySpawnErea[EREA_NUM];