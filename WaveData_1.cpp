#include "WaveData_1.h"

// �E�F�[�u�P
WaveScript wave1Script[] =
{
	SET_ENEMY(3.0f, EREA0, ENEMY_0),
	SET_ENEMY(6.0f, EREA1, ENEMY_1),
	SET_ENEMY(9.0f, EREA2, ENEMY_3),
	SET_OffLight(10.0f),				// ���C�gOFF
	SET_ENEMY(11.0f, EREA0, ENEMY_0),
	SET_ENEMY(12.0f, EREA1, ENEMY_2),
	SET_ENEMY(13.0f, EREA2, ENEMY_3),
	SET_OnLight(15.0f),					// ���C�gON
	SET_EnemyDestory(15.0f),			// �S�Ă̓G�j��

	SET_END								// �E�F�[�u�I��

};

// �E�F�[�u2
WaveScript wave2Script[] =
{
	SET_ENEMY(1.0f, EREA3, ENEMY_0),
	SET_ENEMY(2.0f, EREA4, ENEMY_1),
	SET_ENEMY(3.0f, EREA0, ENEMY_0),

	SET_EnemyDestory(15.0f),			// �S�Ă̓G�j��

	SET_END								// �E�F�[�u�I��
};
