#pragma once
#include <memory>
#include "Scene.h"
#include "Library/3D/SkinnedMesh.h"
#include "Library/3D/SkyMap.h"
#include "Library/2D/Sprite.h"
#include "Pause.h"
#include "Result.h"

// テストシーン
class SceneWave : public Scene
{
public:
	SceneWave() {}
	~SceneWave() override {};

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
	std::unique_ptr<SkyMap> skyMap;

	float gaussianPower = 16.0f;

	// --- ポーズ画面 ---
	std::unique_ptr<Pause> pause;

	// --- リザルト画面 ---
	std::unique_ptr<Result> result;
};
