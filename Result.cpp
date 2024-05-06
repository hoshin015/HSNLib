#include "Result.h"
#include "SpinningTopPlayerManager.h"
#include "Library/Timer.h"

Result::Result()
{
	float deltaRightX = 1300;
	float deltaLeftX = -1300;

	sprMainMenu = std::make_unique<UiSprite>(L"Data/Texture/Result/mainMenu.png");
	sprMainMenu->showPosition = { 100, 550 };
	sprMainMenu->hidePosition = { sprMainMenu->showPosition.x + deltaRightX, sprMainMenu->showPosition.y };
	sprMainMenu->pos = sprMainMenu->hidePosition;
	sprMainMenu->size = { 435 * 0.666, 133 * 0.666 };

	sprSideBar1 = std::make_unique<UiSprite>(L"Data/Texture/Result/sideBar1.png");
	sprSideBar1->showPosition = { 0, 0 };
	sprSideBar1->hidePosition = { sprSideBar1->showPosition.x + deltaLeftX, sprSideBar1->showPosition.y };
	sprSideBar1->pos = sprSideBar1->hidePosition;
	sprSideBar1->size = { 523 * 0.666, 585 * 0.666 };


	sprSideBar2 = std::make_unique<UiSprite>(L"Data/Texture/Result/sideBar2.png");
	sprSideBar2->showPosition = { 378, 120 };
	sprSideBar2->hidePosition = { sprSideBar2->showPosition.x + deltaRightX, sprSideBar2->showPosition.y };
	sprSideBar2->pos = sprSideBar2->hidePosition;
	sprSideBar2->size = { 1355 * 0.666, 901 * 0.666 };
}

void Result::Update()
{
	// プレイヤー死亡チェック
	if (SpinningTopPlayerManager::Instance().GetPlayerCount() <= 0) return;
	if (SpinningTopPlayerManager::Instance().GetPlayer(0)->isDead)
	{
		isResult = true;
	}
	else
	{
		return;
	}
	
	waitTimer += Timer::Instance().DeltaTime();
	if (waitTimer < waitTime) return;


	if (isResult)
	{
		sprMainMenu->showUi = true;
		sprSideBar1->showUi = true;
		sprSideBar2->showUi = true;
	}

	sprMainMenu->UiUpdate();
	sprSideBar1->UiUpdate();
	sprSideBar2->UiUpdate();
}

void Result::Render()
{
	sprMainMenu->UiRender();
	sprSideBar1->UiRender();
	sprSideBar2->UiRender();
}
