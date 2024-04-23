#pragma once
#include <memory>
#include "SpinningTopEnemyManager.h"
#include "StEnemy.h"

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
	ScriptEnemy(DirectX::XMFLOAT3 p) : position(p) {}
	DirectX::XMFLOAT3 position;
	int type;

	void Execute() override
	{
		StEnemy* enemy = new StEnemy(ENEMY_0);
		enemy->SetPosition(position);
		// スポーン座標設定(徘徊用)
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