#pragma once

#include "Enemy.h"


class EnemySlime : public Enemy
{
public:
	EnemySlime();
	~EnemySlime() override;

	// XVˆ—
	void Update();

	// •`‰æˆ—
	void Render();

protected:
	// €–Sˆ—
	void OnDead() override;

private:
	float radius = 0.5f;
	float height = 1.0f;

};