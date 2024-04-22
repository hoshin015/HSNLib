#pragma once

enum ENEMY_TYPE
{
	ENEMY_0,
	ENEMY_1,

	ENEMY_NUM,
};

struct EnemyData
{
	int enemyType;	// “G‚ÌŽí—Þ
	float radius;	// “G‚Ì”¼Œa

	float pursuitRadius;
	float searchRadius;
	float notSearchRadius;
};

extern EnemyData enemyData[];