#pragma once
#include "UiSprite.h"

class Pause
{
public:
	Pause();
	~Pause() {}

	// �X�V
	void Update();

	// �`��
	void Render();

	// --- �|�[�Y��� ---
	bool isPause = false;
	std::unique_ptr<UiSprite> sprGiveUp;
	std::unique_ptr<UiSprite> sprGameRestart;
	std::unique_ptr<UiSprite> sprRetry;
	std::unique_ptr<UiSprite> sprSideBar1;
	std::unique_ptr<UiSprite> sprSideBar2;

	// ���j���[
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