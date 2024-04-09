#pragma once

#include "Enemy.h"


class EnemySlime : public Enemy
{
public:
	EnemySlime();
	~EnemySlime() override;

	// �X�V����
	void Update();

	// �`�揈��
	void Render();

protected:
	// ���S����
	void OnDead() override;

private:
	float radius = 0.5f;
	float height = 1.0f;

};