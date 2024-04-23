#pragma once
#include <memory>
#include "SpinningTopEnemyManager.h"
#include "StEnemy.h"

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
	ScriptEnemy(DirectX::XMFLOAT3 p) : position(p) {}
	DirectX::XMFLOAT3 position;
	int type;

	void Execute() override
	{
		StEnemy* enemy = new StEnemy(ENEMY_0);
		enemy->SetPosition(position);
		// �X�|�[�����W�ݒ�(�p�j�p)
		enemy->spawnPosition = { enemy->GetPosition().x, 0.0f, enemy->GetPosition().z };
		SpinningTopEnemyManager::Instance().Register(enemy);
	}
};

#define SET_ENEMY(time, position){(time), std::make_uniqu<ScriptData>(new ScriptEnemy({0,0,0})6}


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