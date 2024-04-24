#pragma once
#include <memory>
#include "SpinningTopEnemyManager.h"
#include "StEnemy.h"
#include "StEnemyData.h"
#include <random>

struct ScriptData
{
	virtual ~ScriptData() = default;
	virtual void Execute() = 0;			// 生成時に実行する関数
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
		// 乱数生成器の設定
		std::random_device rd;  // 真の乱数生成器を初期化
		std::mt19937 gen(rd()); // メルセンヌ・ツイスタ乱数生成器を初期化
		std::uniform_real_distribution<float> dis(0.0f, enemySpawnErea[spawnEreaType].radius); // 0からenemySpawnErea[i].radiusの間の一様乱数生成器を作成

		StEnemy* enemy = new StEnemy(type);

		float xPos = enemySpawnErea[spawnEreaType].position.x;
		float zPos = enemySpawnErea[spawnEreaType].position.z;

		float rad = DirectX::XMConvertToRadians(rand() % 360);
		float dist = dis(gen);

		xPos += cosf(rad) * dist;
		zPos += sinf(rad) * dist;

		enemy->SetPosition({ xPos, 10, zPos });

		// スポーン座標設定
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

	// 初期化処理
	void Init();

	// 更新処理
	void Update();

public:
	float timer;
	int state = 0;

	WaveScript* pScript = nullptr;
	WaveScript** ppScript = nullptr;
};