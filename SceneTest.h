#pragma once
#include <memory>
#include "Scene.h"
#include "Library/2D/Sprite.h"


// テストシーン
class SceneTest : public Scene
{
public:
	SceneTest() {}
	~SceneTest() override {};

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新
	void Update() override;

	// 描画
	void Render() override;

	// デバッグ描画
	void DrawDebugGUI();

private:
	std::unique_ptr<Sprite> sprite;

	float timer = 0;
	float time = 2.0f;

	float nowPos = 0;
	float startPos = 0;
	float endPos = 640;
};
