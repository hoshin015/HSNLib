#pragma once
#include <memory>
#include "Scene.h"
#include "Library/2D/Sprite.h"
#include "Library/2D/UVScrollSprite.h"
#include "Library/2D/MaskSprite.h"


// タイトルシーン
class SceneTitle : public Scene
{
public:
	SceneTitle() {}
	~SceneTitle() override {};

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
	Sprite* sprite = nullptr;

	std::unique_ptr<Sprite> testText;

	std::unique_ptr<UVScrollSprite> uvSprite;
	DirectX::XMFLOAT2 uvScrollValue = { 0,0 };

	std::unique_ptr<MaskSprite> maskSprite;
};
