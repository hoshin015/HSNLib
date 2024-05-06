#pragma once
#include "UiSprite.h"

class Result
{
public:
	Result();
	~Result() {};

	// 更新
	void Update();

	// 描画
	void Render();

	// --- リザルト画面 ---
	bool isResult = false;
	float waitTime = 2.0f;		// プレイヤー死亡からの待ち時間
	float waitTimer = 0.0f;		// プレイヤー死亡からすぐにUI表示をしないために使う
	std::unique_ptr<UiSprite> sprMainMenu;
	std::unique_ptr<UiSprite> sprSideBar1;
	std::unique_ptr<UiSprite> sprSideBar2;
};