#pragma once
#include "SpinningTopEnemy.h"
#include "StEnemyData.h"

#include "ParryEffect.h"

class StEnemy : public SpinningTopEnemy
{
public:
	StEnemy(int enemyKind);
	~StEnemy() override;

	// �X�V����
	void Update() override;

	// �`�揈��
	void Render(bool drawShadow = false) override;

	// �f�o�b�O�v���~�e�B�u�`��
	void DrawDebugPrimitive() override;

	// TargetPosition �X�V
	void UpdateTargetPosition();

private:
	void CreateAiTree();


protected:
	// ���S����
	void OnDead() override;

private:
	float radius = 0.5f;
	float height = 1.0f;

	std::unique_ptr<ParryEffect> pF;
};