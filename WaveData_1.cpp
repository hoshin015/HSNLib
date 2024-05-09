#include "WaveData_1.h"
#include "ObsMarunoko.h"

// �X�e�[�W�̃T�C�Y�� 100 x 100�@(-50 ~ +50 �̊�)

// �E�F�[�u�P
WaveScript wave1Script[] =
{
	SET_ENEMY(1.0f, EREA1, ENEMY_3),
	SET_ENEMY(1.5f, EREA0, ENEMY_0),
	SET_ENEMY(1.5f, EREA3, ENEMY_0),

	SET_ENEMY(6.0f, EREA2, ENEMY_0),
	SET_ENEMY(6.0f, EREA2, ENEMY_2),

	SET_ENEMY(10.0f, EREA4, ENEMY_0),
	SET_ENEMY(10.0f, EREA4, ENEMY_2),

	SET_ENEMY(22.0f, EREA1, ENEMY_0),
	SET_ENEMY(22.0f, EREA1, ENEMY_0),
	SET_ENEMY(22.0f, EREA0, ENEMY_2),
	SET_ENEMY(28.0f, EREA0, ENEMY_2),
	SET_ENEMY(28.0f, EREA3, ENEMY_3),
	SET_ENEMY(28.0f, EREA3, ENEMY_3),
	SET_ENEMY(28.0f, EREA3, ENEMY_3),

	SET_ENEMY(35.0f, EREA1, ENEMY_0),
	SET_ENEMY(35.0f, EREA1, ENEMY_0),
	SET_ENEMY(35.0f, EREA1, ENEMY_0),
	SET_ENEMY(35.0f, EREA0, ENEMY_2),
	SET_ENEMY(35.0f, EREA0, ENEMY_2),
	SET_ENEMY(35.0f, EREA3, ENEMY_3),
	SET_ENEMY(35.0f, EREA3, ENEMY_3),

	SET_ENEMY(42.0f, EREA0, ENEMY_1),
	SET_ENEMY(42.0f, EREA3, ENEMY_1),
	SET_ENEMY(42.0f, EREA3, ENEMY_0),


	SET_EnemyDestory(50.0f),			// �S�Ă̓G�j��
	SET_ObstacleDestory(50.0f),			// �j��������Ă����Q���̔j��

	SET_END								// �E�F�[�u�I��

};


// �E�F�[�u2
WaveScript wave2Script[] =
{
	SET_MARUNOKO(0.0f, ObsMarunoko::MarunokoType::NO_MOVE, 20, 0, 0, 0),		// ��]�n���� 
	SET_MARUNOKO(0.0f, ObsMarunoko::MarunokoType::NO_MOVE, 0, 0, 20, 0),		// ��]�n���� 
	SET_MARUNOKO(0.0f, ObsMarunoko::MarunokoType::NO_MOVE, 0, 0, -20, 0),		// ��]�n���� 
	SET_MARUNOKO(0.0f, ObsMarunoko::MarunokoType::NO_MOVE, -20, 0, 0, 0),		// ��]�n���� 


	SET_MARUNOKO(0.0f, ObsMarunoko::MarunokoType::HORIZONTAL, 60, 0, 10, 3),	// ��]�n���� 
	SET_MARUNOKO(0.0f, ObsMarunoko::MarunokoType::VERTICAL, 10, 0, -60, 3),	// ��]�n���� 


	SET_ENEMY(1.0f, EREA0, ENEMY_1),
	SET_ENEMY(1.0f, EREA1, ENEMY_1),
	SET_ENEMY(5.0f, EREA2, ENEMY_2),

	SET_ENEMY(9.0f, EREA0, ENEMY_0),
	SET_ENEMY(12.0f, EREA4, ENEMY_1),
	SET_ENEMY(12.0f, EREA1, ENEMY_1),
	SET_ENEMY(12.0f, EREA3, ENEMY_2),


	SET_ENEMY(19.0f, EREA0, ENEMY_1),
	SET_ENEMY(19.0f, EREA1, ENEMY_1),
	SET_ENEMY(19.0f, EREA2, ENEMY_0),
	SET_ENEMY(19.0f, EREA3, ENEMY_2),
	SET_ENEMY(19.0f, EREA4, ENEMY_3),


	SET_EnemyDestory(50.0f),			// �S�Ă̓G�j��
	SET_ObstacleDestory(50.0f),			// �j��������Ă����Q���̔j��

	SET_END								// �E�F�[�u�I��
};

// �E�F�[�u3
WaveScript wave3Script[] =
{

	SET_ENEMY(1.0f, EREA0, ENEMY_0),
	SET_ENEMY(2.0f, EREA1, ENEMY_2),
	SET_ENEMY(3.0f, EREA2, ENEMY_3),


	SET_OffLight(3.0f),				// ���C�gOFF

	SET_ENEMY(5.0f, EREA0, ENEMY_1),
	SET_ENEMY(5.0f, EREA1, ENEMY_1),
	SET_ENEMY(8.0f, EREA2, ENEMY_2),

	SET_OnLight(12.0f),					// ���C�gON

	SET_ENEMY(15.0f, EREA0, ENEMY_2),
	SET_ENEMY(15.0f, EREA4, ENEMY_3),
	SET_ENEMY(15.0f, EREA3, ENEMY_1),

	SET_OffLight(15.0f),				// ���C�gOFF

	SET_ENEMY(19.0f, EREA0, ENEMY_1),
	SET_ENEMY(19.0f, EREA1, ENEMY_1),
	SET_ENEMY(19.0f, EREA2, ENEMY_0),
	SET_ENEMY(19.0f, EREA3, ENEMY_2),
	SET_ENEMY(19.0f, EREA4, ENEMY_3),

	SET_OnLight(24.0f),					// ���C�gON

	SET_ENEMY(26.0f, EREA1, ENEMY_2),
	SET_ENEMY(29.0f, EREA0, ENEMY_3),
	SET_ENEMY(29.0f, EREA0, ENEMY_3),

	SET_OffLight(35.0f),				// ���C�gOFF

	SET_OnLight(50.0f),					// ���C�gON

	SET_EnemyDestory(50.0f),			// �S�Ă̓G�j��
	SET_ObstacleDestory(50.0f),			// �j��������Ă����Q���̔j��

	SET_END								// �E�F�[�u�I��
};

// �E�F�[�u4
WaveScript wave4Script[] =
{
	SET_MARUNOKO(0.0f, ObsMarunoko::MarunokoType::NO_MOVE, 30, 0, 0, 0),		// ��]�n���� 
	SET_MARUNOKO(0.0f, ObsMarunoko::MarunokoType::NO_MOVE, 0, 0, 30, 0),		// ��]�n���� 
	SET_MARUNOKO(0.0f, ObsMarunoko::MarunokoType::NO_MOVE, 0, 0, -30, 0),		// ��]�n���� 
	SET_MARUNOKO(0.0f, ObsMarunoko::MarunokoType::NO_MOVE, -30, 0, 0, 0),		// ��]�n���� 

	SET_ENEMY(1.0f, EREA2, ENEMY_0),
	SET_ENEMY(2.0f, EREA2, ENEMY_2),
	SET_ENEMY(3.0f, EREA4, ENEMY_3),
	SET_ENEMY(3.0f, EREA4, ENEMY_3),


	SET_OffLight(3.0f),				// ���C�gOFF

	SET_ENEMY(5.0f, EREA0, ENEMY_1),
	SET_ENEMY(5.0f, EREA1, ENEMY_1),
	SET_ENEMY(8.0f, EREA2, ENEMY_2),

	SET_OnLight(12.0f),					// ���C�gON

	SET_ENEMY(15.0f, EREA0, ENEMY_2),
	SET_ENEMY(15.0f, EREA4, ENEMY_3),
	SET_ENEMY(15.0f, EREA3, ENEMY_1),

	SET_OffLight(15.0f),				// ���C�gOFF

	SET_ENEMY(19.0f, EREA0, ENEMY_1),
	SET_ENEMY(19.0f, EREA1, ENEMY_1),
	SET_ENEMY(19.0f, EREA2, ENEMY_0),
	SET_ENEMY(19.0f, EREA3, ENEMY_2),
	SET_ENEMY(19.0f, EREA4, ENEMY_3),

	SET_OnLight(24.0f),					// ���C�gON

	SET_MARUNOKO(0.0f, ObsMarunoko::MarunokoType::HORIZONTAL, 60, 0, 10, 8),	// ��]�n���� 
	SET_MARUNOKO(0.0f, ObsMarunoko::MarunokoType::VERTICAL, 10, 0, -60, 8),	// ��]�n���� 


	SET_EnemyDestory(50.0f),			// �S�Ă̓G�j��
	SET_ObstacleDestory(50.0f),			// �j��������Ă����Q���̔j��

	SET_END								// �E�F�[�u�I��
};

// �E�F�[�u5
WaveScript wave5Script[] =
{
	SET_END								// �E�F�[�u�I��
};

// �E�F�[�u6
WaveScript wave6Script[] =
{
	SET_END								// �E�F�[�u�I��
};

// �E�F�[�u7
WaveScript wave7Script[] =
{
	SET_END								// �E�F�[�u�I��
};

// �E�F�[�u8
WaveScript wave8Script[] =
{
	SET_END								// �E�F�[�u�I��
};

// �E�F�[�u9
WaveScript wave9Script[] =
{
	SET_END								// �E�F�[�u�I��
};

// �E�F�[�u10
WaveScript wave10Script[] =
{
	SET_END								// �E�F�[�u�I��
};




//#include "WaveData_1.h"
//#include "ObsMarunoko.h"
//
//// �X�e�[�W�̃T�C�Y�� 100 x 100�@(-50 ~ +50 �̊�)
//
//// �E�F�[�u�P
//WaveScript wave1Script[] =
//{
//	SET_MARUNOKO(0.0f, ObsMarunoko::MarunokoType::NO_MOVE, 20, 0, 0, 0),		// ��]�n���� 
//	SET_MARUNOKO(0.0f, ObsMarunoko::MarunokoType::HORIZONTAL, 60, 0, 10, 3),	// ��]�n���� 
//	SET_MARUNOKO(0.0f, ObsMarunoko::MarunokoType::VERTICAL, 10, 0, -60, 10),	// ��]�n���� 
//
//	SET_ENEMY(3.0f, EREA0, ENEMY_0),
//	SET_ENEMY(6.0f, EREA1, ENEMY_1),
//	SET_ENEMY(9.0f, EREA2, ENEMY_3),
//	SET_OffLight(10.0f),				// ���C�gOFF
//	SET_ENEMY(11.0f, EREA0, ENEMY_0),
//	SET_ENEMY(12.0f, EREA1, ENEMY_2),
//	SET_ENEMY(13.0f, EREA2, ENEMY_3),
//	SET_OnLight(15.0f),					// ���C�gON
//
//	SET_EnemyDestory(15.0f),			// �S�Ă̓G�j��
//	SET_ObstacleDestory(15.0f),			// �j��������Ă����Q���̔j��
//
//	SET_END								// �E�F�[�u�I��
//};
//
//// �E�F�[�u2
//WaveScript wave2Script[] =
//{
//	SET_ENEMY(1.0f, EREA3, ENEMY_0),
//	SET_ENEMY(2.0f, EREA4, ENEMY_1),
//	SET_ENEMY(3.0f, EREA0, ENEMY_0),
//
//	SET_EnemyDestory(15.0f),			// �S�Ă̓G�j��
//	SET_ObstacleDestory(15.0f),			// �j��������Ă����Q���̔j��
//
//	SET_END								// �E�F�[�u�I��
//};
//
//// �E�F�[�u3
//WaveScript wave3Script[] =
//{
//	SET_END								// �E�F�[�u�I��
//};
//
//// �E�F�[�u4
//WaveScript wave4Script[] =
//{
//	SET_END								// �E�F�[�u�I��
//};
//
//// �E�F�[�u5
//WaveScript wave5Script[] =
//{
//	SET_END								// �E�F�[�u�I��
//};
//
//// �E�F�[�u6
//WaveScript wave6Script[] =
//{
//	SET_END								// �E�F�[�u�I��
//};
//
//// �E�F�[�u7
//WaveScript wave7Script[] =
//{
//	SET_END								// �E�F�[�u�I��
//};
//
//// �E�F�[�u8
//WaveScript wave8Script[] =
//{
//	SET_END								// �E�F�[�u�I��
//};
//
//// �E�F�[�u9
//WaveScript wave9Script[] =
//{
//	SET_END								// �E�F�[�u�I��
//};
//
//// �E�F�[�u10
//WaveScript wave10Script[] =
//{
//	SET_END								// �E�F�[�u�I��
//};