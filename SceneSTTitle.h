#pragma once
#include <memory>
#include <vector>
#include "Scene.h"
#include "Library/2D/Sprite.h"
#include "Library/2D/MaskSprite.h"
#include "Library/2D/Primitive2D.h"

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
	std::vector<std::unique_ptr<Sprite>> sprite;
	std::unique_ptr<MaskSprite> maskSprite;
	Primitive2D fadeout;
	float fadeoutA = 0;
	DirectX::XMFLOAT4 _videoColor = { 1,1,1,1 };
	float rotate = 0;

	DirectX::XMFLOAT2 dPos = {};
	DirectX::XMFLOAT2 dPos2 = {};
	DirectX::XMFLOAT2 dSize = {};
	DirectX::XMFLOAT2 dSize2 = {};
};
