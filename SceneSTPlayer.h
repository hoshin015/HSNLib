#pragma once
#include <memory>
#include "Scene.h"
#include "Library/3D/SkinnedMesh.h"
#include "StageContext.h"

// テストシーン
class SceneSTPlayer : public Scene {
public:
	SceneSTPlayer() {}
	~SceneSTPlayer() override {};

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
	std::unique_ptr<StageContext> stageMain;
};
