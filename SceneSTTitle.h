#pragma once
#include <memory>
#include "Scene.h"
#include "Library/2D/Sprite.h"
#include "Library/2D/UVScrollSprite.h"
#include "Library/2D/MaskSprite.h"


// タイトルシーン
class SceneSTTitle : public Scene {
public:
	SceneSTTitle() {}
	~SceneSTTitle() override {};

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
	DirectX::XMFLOAT2 _videoPos = {0,0};
	DirectX::XMFLOAT2 _videoSize = { 1280,720 };
	DirectX::XMFLOAT4 _videoColor = { 1,1,1,1 };
};
