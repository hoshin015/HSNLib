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
	int enemyKind;					// “G‚Ìí—Ş
	int behaviorType;				// ƒrƒwƒCƒrƒA‚Ìí—Ş

	float radius;					// “G‚Ì”¼Œa
	float maxMoveSpeed;				// Å‘åˆÚ“®‘¬“x
	float maxHealth;				// Å‘åHP

	float pursuitRadius;			// pursuit”ÍˆÍ
	float searchRadius;				// õ“G”ÍˆÍ
	float notSearchRadius;			// ”ñõ“G”ÍˆÍ

	float circleDistance;			// wander‰~‚Æ‚Ì‹——£
	float circleRadius;				// wander‰~‚Ì”¼Œa
	int wanderAngleChange;			// wander‰~‚Ì•ÏXŠp“x
	float wanderAngleChangeTime;	// wander‰~‚Ì•ÏX•b”

	float chargeAttackCoolTime;		// chargeAttack‚ÌƒN[ƒ‹ƒ^ƒCƒ€•b”

	float waitChargeAttackTime;		// chargeAttack‚Ì‘Ò‚¿ŠÔ
};

extern EnemyData enemyData[ENEMY_NUM];

struct EnemySpawnErea
{
	DirectX::XMFLOAT3 position;
	float radius = 3;
};

extern EnemySpawnErea enemySpawnErea[EREA_NUM];