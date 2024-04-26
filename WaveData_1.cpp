#include "WaveData_1.h"

WaveScript wave1Script[] =
{
	SET_ENEMY(3.0f, EREA0, ENEMY_0),
	SET_ENEMY(6.0f, EREA1, ENEMY_2),
	SET_ENEMY(9.0f, EREA2, ENEMY_3),
	SET_OffLight(10.0f),
	SET_ENEMY(11.0f, EREA0, ENEMY_0),
	SET_ENEMY(12.0f, EREA1, ENEMY_2),
	SET_ENEMY(13.0f, EREA2, ENEMY_3),

	SET_END								// 終了コード

};

WaveScript wave2Script[] =
{
	SET_OnLight(0.0f),
	SET_ENEMY(1.0f, EREA3, ENEMY_0),
	SET_ENEMY(2.0f, EREA4, ENEMY_1),
	SET_ENEMY(3.0f, EREA0, ENEMY_0),

	SET_END								// 終了コード
};