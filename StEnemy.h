#pragma once
#include "SpinningTopEnemy.h"
#include "StEnemyData.h"

#include "ParryEffect.h"

class StEnemy : public SpinningTopEnemy
{
public:
	StEnemy(int enemyKind);
	~StEnemy() override;

	// 更新処理
	void Update() override;

	// 描画処理
	void Render(bool drawShadow = false) override;

	// デバッグプリミティブ描画
	void DrawDebugPrimitive() override;

	// TargetPosition 更新
	void UpdateTargetPosition();

private:
	void CreateAiTree();


protected:
	// 死亡処理
	void OnDead() override;

private:
	float radius = 0.5f;
	float height = 1.0f;

	std::unique_ptr<ParryEffect> pF;
};