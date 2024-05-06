#pragma once
#include "UiSprite.h"

class Pause
{
public:
	Pause();
	~Pause() {}

	// 更新
	void Update();

	// 描画
	void Render();

	// --- ポーズ画面 ---
	bool isPause = false;
	std::unique_ptr<UiSprite> sprGiveUp;
	std::unique_ptr<UiSprite> sprGameRestart;
	std::unique_ptr<UiSprite> sprRetry;
	std::unique_ptr<UiSprite> sprSideBar1;
	std::unique_ptr<UiSprite> sprSideBar2;

	// メニュー
	enum class selectMenu
	{
		RESTART,
		RETRY,
		GIVEUP,
		END,
	};
	int selectNum = static_cast<int>(selectMenu::RESTART);
	std::unique_ptr<UiSprite> sprSelect0;
	std::unique_ptr<UiSprite> sprSelect1;
	std::unique_ptr<UiSprite> sprSelect2;
};