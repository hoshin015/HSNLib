#pragma once

#include "Enemy.h"


class EnemySlime : public Enemy
{
public:
	EnemySlime();
	~EnemySlime() override;

	// 更新処理
	void Update();

	// 描画処理
	void Render();

protected:
	// 死亡処理
	void OnDead() override;

private:
	float radius = 0.5f;
	float height = 1.0f;

};