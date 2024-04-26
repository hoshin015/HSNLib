#pragma once
#include <memory>
#include "Scene.h"
#include "Library/3D/SkinnedMesh.h"
#include "Library/3D/SkyMap.h"

// テストシーン
class SceneSpinningTop : public Scene
{
public:
	SceneSpinningTop() {}
	~SceneSpinningTop() override {};

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
};
