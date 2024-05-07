#pragma once
#include <memory>
#include <random>
#include "SpinningTopEnemyManager.h"
#include "StEnemy.h"
#include "StEnemyData.h"
#include "LightManager.h"
#include "ObsMarunoko.h"
#include "ObstacleManager.h"
#include "Library/2D/Sprite.h"

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

// �G����
struct ScriptEnemy : public ScriptData
{
	ScriptEnemy(int sType, int eType) : spawnType(sType), enemyType(eType) {};
	int spawnType;
	int enemyType;

	void Execute() override
	{
		// ����������̐ݒ�
		std::random_device rd;  // �^�̗����������������
		std::mt19937 gen(rd()); // �����Z���k�E�c�C�X�^�����������������
		std::uniform_real_distribution<float> dis(0.0f, enemySpawnErea[spawnType].radius); // 0����enemySpawnErea[i].radius�̊Ԃ̈�l������������쐬

		StEnemy* enemy = new StEnemy(enemyType);

		float xPos = enemySpawnErea[spawnType].position.x;
		float zPos = enemySpawnErea[spawnType].position.z;

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

struct ScriptMarunoko : public ScriptData
{
	ScriptMarunoko(int mType, DirectX::XMFLOAT3 pos, float speed) : type(mType), position(pos), moveSpeed(speed) {};
	int type;
	DirectX::XMFLOAT3 position;
	float moveSpeed;

	void Execute() override
	{
		ObsMarunoko* obstacle = new ObsMarunoko("Data/FBX/StMarunoko/StMarunoko.fbx", type, moveSpeed);
		obstacle->SetPosition(position);
		ObstacleManager::Instance().Register(obstacle);
	}
};

// ���C�gON
struct ScriptOnLight : public ScriptData
{
	ScriptOnLight() {}

	void Execute() override
	{
		LightManager::Instance().Clear();

		// ���C�g������
		Light* directionLight = new Light(LightType::Directional);
		directionLight->SetDirection(DirectX::XMFLOAT3(0.5, -1, -1));
		directionLight->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
		LightManager::Instance().Register(directionLight);

		LightManager::Instance().SetAmbientColor({ 0.2f, 0.2f, 0.2f, 1.0f });
	}
};

// ���C�gOFF
struct ScriptOffLight : public ScriptData
{
	ScriptOffLight() {}

	void Execute() override
	{
		LightManager::Instance().Clear();

		// ���C�g������
		Light* directionLight = new Light(LightType::Directional);
		directionLight->SetDirection(DirectX::XMFLOAT3(0.5, -1, -1));
		directionLight->SetColor(DirectX::XMFLOAT4(0, 0, 0, 1));
		LightManager::Instance().Register(directionLight);

		// �_�����ǉ�
		{
			for (int i = 0; i < 2; i++)
			{
				for (int j = 0; j < 2; j++)
				{
					Light* light = new Light(LightType::Point);
					light->SetPosition({ i * 50.0f - 25.0f, 7, j * 50.0f - 25.0f });
					light->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
					light->SetRange(30.0f);
					LightManager::Instance().Register(light);
				}
			}
		}

		LightManager::Instance().SetAmbientColor({ 0,0,0,1.0f });
	}
};

struct SciptDestoryEnemy : public ScriptData
{
	SciptDestoryEnemy() {}
	void Execute() override
	{
		SpinningTopEnemyManager::Instance().Clear();
	}
};

struct ScriptDestoryObstacle : public ScriptData
{
	ScriptDestoryObstacle() {}
	void Execute() override
	{
		ObstacleManager::Instance().WaveClear();
	}
};

#define SET_ENEMY(time, spawnType, enemyType) {(time), std::unique_ptr<ScriptData>(new ScriptEnemy(spawnType, enemyType))}
#define SET_MARUNOKO(time, marunokoType, posX, posY, posZ, speed) {(time), std::unique_ptr<ScriptData>(new ScriptMarunoko(marunokoType, {posX, posY, posZ}, speed))}
#define SET_OnLight(time) {(time), std::unique_ptr<ScriptData>(new ScriptOnLight())}
#define SET_OffLight(time) {(time), std::unique_ptr<ScriptData>(new ScriptOffLight())}
#define SET_EnemyDestory(time) {(time), std::unique_ptr<ScriptData>(new SciptDestoryEnemy())}
#define SET_ObstacleDestory(time) {(time), std::unique_ptr<ScriptData>(new ScriptDestoryObstacle())}
#define SET_END		{0,nullptr}

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
	float timer = 0;
	int state = 0;

	WaveScript* pScript = nullptr;
	WaveScript** ppScript = nullptr;

	int nowWave = 0;				// ���݂̃E�F�[�u
	float waveLimitTime = 50.0f;	// �E�F�[�u�̎���50�b
	float waveLimitTimer = 0.0f;	// �E�F�[�u�̎c�莞�ԃ^�C�}�[

	// �E�F�[�u�\���p
	float showWaveTextEaseTime = 0.3f;	// �C�[�W���O�Ɏg�p���鎞��
	float showWaveTextWaitTime = 1.2f;	// �����ł������ړ����鎞��
	float showWaveTextTimer = 0.0f;		// �E�F�[�u�\���̑S�̎��Ԃ̊Ǘ��p�^�C�}�[

	std::unique_ptr<Sprite> sprWaveCut1;
};