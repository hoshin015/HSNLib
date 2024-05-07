#include "Result.h"
#include "SpinningTopPlayerManager.h"
#include "Library/Timer.h"
#include "Library/Input/InputManager.h"
#include "SceneManager.h"



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


	sprRankS = std::make_unique<UiSprite>(L"Data/Texture/Result/rankS.png");
	sprRankS->showPosition = { 1050, 150 };
	sprRankS->hidePosition = { sprRankS->showPosition.x + deltaRightX, sprRankS->showPosition.y };
	sprRankS->pos = sprRankS->hidePosition;
	sprRankS->size = { 256 * 0.666, 256 * 0.666 };

	sprTotalScore = std::make_unique<UiSprite>(L"Data/Font/fontResult.png");
	sprTotalScore->pos = { 940,375 };
	sprTotalScore->easingTime = 1.0f;
	
	sprAliveTime = std::make_unique<UiSprite>(L"Data/Font/fontResult.png");
	sprAliveTime->pos = { 940,310 };
	sprAliveTime->easingTime = 1.0f;

	sprTotalDestoryEnemy = std::make_unique<UiSprite>(L"Data/Font/fontResult.png");
	sprTotalDestoryEnemy->pos = { 1030,460 };

	sprGetTotalOption = std::make_unique<UiSprite>(L"Data/Font/fontResult.png");
	sprGetTotalOption->pos = { 1030,512 };
	
	sprNowWave = std::make_unique<UiSprite>(L"Data/Font/fontResult.png");
	sprNowWave->pos = { 1030,564 };
	
	sprTotalHitDamage = std::make_unique<UiSprite>(L"Data/Font/fontResult.png");
	sprTotalHitDamage->pos = { 1030,616 };
}

void Result::Update()
{
	// プレイヤー死亡チェック
	if (SpinningTopPlayerManager::Instance().GetPlayerCount() <= 0) return;
	if (SpinningTopPlayerManager::Instance().GetPlayer(0)->isDead)
	{
		isResult = true;

		// 値設定
		sprTotalScore->targetScore = 3000;
		sprAliveTime->targetScore = SpinningTopPlayerManager::Instance().GetPlayer(0)->aliveTime;

		sprTotalDestoryEnemy->targetScore = SpinningTopPlayerManager::Instance().GetPlayer(0)->totalDestoryEnemy;
		sprGetTotalOption->targetScore = SpinningTopPlayerManager::Instance().GetPlayer(0)->getTotalOption;
		sprNowWave->targetScore = 10;
		sprTotalHitDamage->targetScore = SpinningTopPlayerManager::Instance().GetPlayer(0)->totalHitDamage;
	}
	else
	{
		return;
	}
	
	waitTimer += Timer::Instance().DeltaTime();
	if (waitTimer < waitTime) return;

	// 画面遷移
	if (waitTimer > resultFinishTime) resultFinish = true;
	if (InputManager::Instance().GetKeyReleased(Keyboard::Enter))
	{
		if(resultFinish)
			SceneManager::Instance().ChangeScene(new SceneSTTitle);
	}

	if (isResult)
	{
		sprMainMenu->showUi = true;
		sprSideBar1->showUi = true;
		sprSideBar2->showUi = true;

		sprRankS->showUi = true;

		sprTotalScore->showUi = true;
		sprAliveTime->showUi = true;

		sprTotalDestoryEnemy->showUi = true;
		sprGetTotalOption->showUi = true;
		sprNowWave->showUi = true;
		sprTotalHitDamage->showUi = true;
	}

	sprMainMenu->UiUpdate();
	sprSideBar1->UiUpdate();
	sprSideBar2->UiUpdate();

	if (waitTimer > startRank) sprRankS->UiUpdate();

	if (waitTimer > startTotalScore) sprTotalScore->UiTextUpdate();
	if (waitTimer > startAliveTime) sprAliveTime->UiTextTimeUpdate();
	if (waitTimer > startTotalDestoryEnemy) sprTotalDestoryEnemy->UiTextUpdate();
	if (waitTimer > startTotalGetOption) sprGetTotalOption->UiTextUpdate();
	if (waitTimer > startNowWave) sprNowWave->UiTextUpdate();
	if (waitTimer > startTotalHitDamage) sprTotalHitDamage->UiTextUpdate();
}

void Result::Render()
{
	sprMainMenu->UiRender();
	sprSideBar1->UiRender();
	sprSideBar2->UiRender();


	if (waitTimer > startTotalScore) sprTotalScore->UiTextRender();
	if (waitTimer > startAliveTime) sprAliveTime->UiTextRender();
	if (waitTimer > startTotalDestoryEnemy) sprTotalDestoryEnemy->UiTextRender();
	if (waitTimer > startTotalGetOption)  sprGetTotalOption->UiTextRender();
	if (waitTimer > startNowWave) sprNowWave->UiTextRender();
	if (waitTimer > startTotalHitDamage) sprTotalHitDamage->UiTextRender();
	
	
	if (waitTimer > startRank) sprRankS->UiRender();
}
