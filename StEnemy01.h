#include "SpinningTopEnemy.h"


class StEnemy01 : public SpinningTopEnemy
{
public:
	StEnemy01();
	~StEnemy01() override;

	// 更新処理
	void Update() override;

	// 描画処理
	void Render(bool drawShadow = false) override;

	// デバッグプリミティブ描画
	void DrawDebugPrimitive() override;

	// TargetPosition 更新
	void UpdateTargetPosition();

	// DrawDebugPrimitive


protected:
	// 死亡処理
	void OnDead() override;

private:
	float radius = 0.5f;
	float height = 1.0f;
};