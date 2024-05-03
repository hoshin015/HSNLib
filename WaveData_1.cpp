#include "WaveData_1.h"
#include "ObsMarunoko.h"

// ステージのサイズは 100 x 100　(-50 ~ +50 の間)

// ウェーブ１
WaveScript wave1Script[] =
{
	SET_MARUNOKO(0.0f, ObsMarunoko::MarunokoType::NO_MOVE, 20, 0, 0, 0),		// 回転刃生成 
	SET_MARUNOKO(0.0f, ObsMarunoko::MarunokoType::HORIZONTAL, 36, 0, 10, 3),	// 回転刃生成 
	SET_MARUNOKO(0.0f, ObsMarunoko::MarunokoType::VERTICAL, 10, 0, -60, 10),	// 回転刃生成 

	SET_ENEMY(3.0f, EREA0, ENEMY_0),
	SET_ENEMY(6.0f, EREA1, ENEMY_1),
	SET_ENEMY(9.0f, EREA2, ENEMY_3),
	SET_OffLight(10.0f),				// ライトOFF
	SET_ENEMY(11.0f, EREA0, ENEMY_0),
	SET_ENEMY(12.0f, EREA1, ENEMY_2),
	SET_ENEMY(13.0f, EREA2, ENEMY_3),
	SET_OnLight(15.0f),					// ライトON
	SET_EnemyDestory(15.0f),			// 全ての敵破壊
	SET_ObstacleDestory(15.0f),			// 破壊を許可している障害物の破壊

	SET_END								// ウェーブ終了

};

// ウェーブ2
WaveScript wave2Script[] =
{
	SET_ENEMY(1.0f, EREA3, ENEMY_0),
	SET_ENEMY(2.0f, EREA4, ENEMY_1),
	SET_ENEMY(3.0f, EREA0, ENEMY_0),

	SET_EnemyDestory(15.0f),			// 全ての敵破壊
	SET_ObstacleDestory(15.0f),			// 破壊を許可している障害物の破壊

	SET_END								// ウェーブ終了
};

// ウェーブ3
WaveScript wave3Script[] =
{
	SET_END								// ウェーブ終了
};

// ウェーブ4
WaveScript wave4Script[] =
{
	SET_END								// ウェーブ終了
};

// ウェーブ5
WaveScript wave5Script[] =
{
	SET_END								// ウェーブ終了
};

// ウェーブ6
WaveScript wave6Script[] =
{
	SET_END								// ウェーブ終了
};

// ウェーブ7
WaveScript wave7Script[] =
{
	SET_END								// ウェーブ終了
};

// ウェーブ8
WaveScript wave8Script[] =
{
	SET_END								// ウェーブ終了
};

// ウェーブ9
WaveScript wave9Script[] =
{
	SET_END								// ウェーブ終了
};

// ウェーブ10
WaveScript wave10Script[] =
{
	SET_END								// ウェーブ終了
};