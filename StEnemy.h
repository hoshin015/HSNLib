#pragma once
#include "SpinningTopEnemy.h"
#include "StEnemyData.h"

#include "ParryEffect.h"
#include "EnemySpawnEffect.h"

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
	void CreateModel();

	void CreateAiTree();


protected:
	// ���S����
	void OnDead() override;

private:
	float radius = 0.5f;
	float height = 1.0f;

public:
	std::unique_ptr<ParryEffect> pF;
	std::unique_ptr<EnemySpawnEffect> spawnEffect;

public:
	DirectX::XMFLOAT3 generatePos = { 0,0,0 };
};