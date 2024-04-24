#pragma once
#include <memory>
#include "SpinningTopEnemyManager.h"
#include "StEnemy.h"
#include "StEnemyData.h"
#include <random>

struct ScriptData
{
	virtual ~ScriptData() = default;
	virtual void Execute() = 0;			// �������Ɏ��s����֐�
};

struct WaveScript
{
	float time;
	std::unique_ptr<ScriptData> data;
};

struct ScriptEnemy : public ScriptData
{
	ScriptEnemy(int sType, int eType) : spawnEreaType(sType), type(eType) {};
	int spawnEreaType;
	int type;

	void Execute() override
	{
		// ����������̐ݒ�
		std::random_device rd;  // �^�̗����������������
		std::mt19937 gen(rd()); // �����Z���k�E�c�C�X�^�����������������
		std::uniform_real_distribution<float> dis(0.0f, enemySpawnErea[spawnEreaType].radius); // 0����enemySpawnErea[i].radius�̊Ԃ̈�l������������쐬

		StEnemy* enemy = new StEnemy(type);

		float xPos = enemySpawnErea[spawnEreaType].position.x;
		float zPos = enemySpawnErea[spawnEreaType].position.z;

		float rad = DirectX::XMConvertToRadians(rand() % 360);
		float dist = dis(gen);

		xPos += cosf(rad) * dist;
		zPos += sinf(rad) * dist;

		enemy->SetPosition({ xPos, 10, zPos });

		// �X�|�[�����W�ݒ�
		enemy->spawnPosition = { enemy->GetPosition().x, 0, enemy->GetPosition().z };
		SpinningTopEnemyManager::Instance().Register(enemy);
	}
};


#define SET_ENEMY(time, spawnType, enemyType) {(time), std::unique_ptr<ScriptData>(new ScriptEnemy(spawnType, enemyType))}


class Wave
{
private:
	Wave() {}
	~Wave() {}

public:
	static Wave& Instance()
	{
		static Wave instance;
		return instance;
	}

	// ����������
	void Init();

	// �X�V����
	void Update();

public:
	float timer;
	int state = 0;

	WaveScript* pScript = nullptr;
	WaveScript** ppScript = nullptr;
};