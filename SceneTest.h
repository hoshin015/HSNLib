#pragma once
#include <memory>
#include "Scene.h"
#include "Library/2D/Sprite.h"
#include "Library/2D/UVScrollSprite.h"
#include "Library/2D/MaskSprite.h"


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
};
