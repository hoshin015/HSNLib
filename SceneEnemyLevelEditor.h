#pragma once
#include <memory>
#include "Scene.h"
#include "Library/3D/SkinnedMesh.h"
#include "Library/3D/SkyMap.h"
#include "StEnemyData.h"

// エネミーレベルエディター
class SceneEnemyLevelEditor : public Scene
{
public:
	SceneEnemyLevelEditor() {}
	~SceneEnemyLevelEditor() override {};

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
	// 生成位置更新
	void UpdateGeneratePosition();

private:
	std::unique_ptr<SkyMap> skyMap;

	EnemyData subEnemyData;

	DirectX::XMFLOAT3 generatePosition = { 0,0,0 };
};
