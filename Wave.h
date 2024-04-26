#pragma once
#include <memory>
#include <random>
#include "SpinningTopEnemyManager.h"
#include "StEnemy.h"
#include "StEnemyData.h"
#include "LightManager.h"

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
	ScriptEnemy(int sType, int eType) : spawnType(sType), enemyType(eType) {};
	int spawnType;
	int enemyType;

	void Execute() override
	{
		// 乱数生成器の設定
		std::random_device rd;  // 真の乱数生成器を初期化
		std::mt19937 gen(rd()); // メルセンヌ・ツイスタ乱数生成器を初期化
		std::uniform_real_distribution<float> dis(0.0f, enemySpawnErea[spawnType].radius); // 0からenemySpawnErea[i].radiusの間の一様乱数生成器を作成

		StEnemy* enemy = new StEnemy(enemyType);

		float xPos = enemySpawnErea[spawnType].position.x;
		float zPos = enemySpawnErea[spawnType].position.z;

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

struct ScriptOnLight : public ScriptData
{
	ScriptOnLight() {}

	void Execute() override
	{
		LightManager::Instance().Clear();

		// ライト初期化
		Light* directionLight = new Light(LightType::Directional);
		directionLight->SetDirection(DirectX::XMFLOAT3(0.5, -1, -1));
		directionLight->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
		LightManager::Instance().Register(directionLight);
	}
};

struct ScriptOffLight : public ScriptData
{
	ScriptOffLight() {}

	void Execute() override
	{
		LightManager::Instance().Clear();

		// ライト初期化
		Light* directionLight = new Light(LightType::Directional);
		directionLight->SetDirection(DirectX::XMFLOAT3(0.5, -1, -1));
		directionLight->SetColor(DirectX::XMFLOAT4(0, 0, 0, 1));
		LightManager::Instance().Register(directionLight);

		// 点光源追加
		{
			for (int i = 0; i < 3; i++)
			{
				Light* light = new Light(LightType::Point);
				light->SetPosition({ i * 30.0f - 45.0f, 5, -4 });
				light->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
				light->SetRange(30.0f);
				LightManager::Instance().Register(light);
			}
		}

		LightManager::Instance().SetAmbientColor({ 0,0,0,1.0f });
	}
};


#define SET_ENEMY(time, spawnType, enemyType) {(time), std::unique_ptr<ScriptData>(new ScriptEnemy(spawnType, enemyType))}
#define SET_OnLight(time) {(time), std::unique_ptr<ScriptData>(new ScriptOnLight())}
#define SET_OffLight(time) {(time), std::unique_ptr<ScriptData>(new ScriptOffLight())}
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

	// 初期化処理
	void Init();

	// 更新処理
	void Update();

public:
	float timer = 0;
	int state = 0;

	WaveScript* pScript = nullptr;
	WaveScript** ppScript = nullptr;

	
};