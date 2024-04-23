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
	int enemyKind;			// “G‚Ìí—Ş
	int behaviorType;		// ƒrƒwƒCƒrƒA‚Ìí—Ş
	float radius;			// “G‚Ì”¼Œa

	float pursuitRadius;	// pursuit”ÍˆÍ
	float searchRadius;		// õ“G”ÍˆÍ
	float notSearchRadius;	// ”ñõ“G”ÍˆÍ
};

extern EnemyData enemyData[];