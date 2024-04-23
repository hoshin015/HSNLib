#pragma once

enum ENEMY_KIND
{
	ENEMY_0,
	ENEMY_1,

	ENEMY_NUM,
};

enum BEHAVIOR_TYPE
{
	pursuit,
	chase,
};

struct EnemyData
{
	int enemyKind;			// �G�̎��
	int behaviorType;		// �r�w�C�r�A�̎��
	float radius;			// �G�̔��a

	float pursuitRadius;	// pursuit�͈�
	float searchRadius;		// ���G�͈�
	float notSearchRadius;	// ����G�͈�
};

extern EnemyData enemyData[];