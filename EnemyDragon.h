#pragma once

#include "Enemy.h"

class EnemyDragon : public Enemy
{
public:
	EnemyDragon();
	~EnemyDragon() override;

	// XVˆ—
	void Update();

	// •`‰æˆ—
	void Render();

protected:
	// €–Sˆ—
	void OnDead() override;

private:
	float height = 1.0f;
};