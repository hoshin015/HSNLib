#pragma once

#include "Enemy.h"

class EnemyDragon : public Enemy
{
public:
	EnemyDragon();
	~EnemyDragon() override;

	// �X�V����
	void Update();

	// �`�揈��
	void Render();

protected:
	// ���S����
	void OnDead() override;

private:
	float height = 1.0f;
};