#include "Result.h"
#include "SpinningTopPlayerManager.h"
#include "Library/Timer.h"
#include "Library/Input/InputManager.h"
#include "SceneManager.h"
#include "Wave.h"

float Result::highScore = 0.0f;

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

	sprRankA = std::make_unique<UiSprite>(L"Data/Texture/Result/rankA.png");
	sprRankA->showPosition = { 1050, 150 };
	sprRankA->hidePosition = { sprRankA->showPosition.x + deltaRightX, sprRankA->showPosition.y };
	sprRankA->pos = sprRankA->hidePosition;
	sprRankA->size = { 256 * 0.666, 256 * 0.666 };

	sprRankB = std::make_unique<UiSprite>(L"Data/Texture/Result/rankB.png");
	sprRankB->showPosition = { 1050, 150 };
	sprRankB->hidePosition = { sprRankB->showPosition.x + deltaRightX, sprRankB->showPosition.y };
	sprRankB->pos = sprRankB->hidePosition;
	sprRankB->size = { 256 * 0.666, 256 * 0.666 };

	sprRankC = std::make_unique<UiSprite>(L"Data/Texture/Result/rankC.png");
	sprRankC->showPosition = { 1050, 150 };
	sprRankC->hidePosition = { sprRankC->showPosition.x + deltaRightX, sprRankC->showPosition.y };
	sprRankC->pos = sprRankC->hidePosition;
	sprRankC->size = { 256 * 0.666, 256 * 0.666 };


	sprHighScore = std::make_unique<UiSprite>(L"Data/Texture/Result/highScore.png");
	sprHighScore->showPosition = { 600, 350 };
	sprHighScore->hidePosition = { sprHighScore->showPosition.x + deltaRightX, sprHighScore->showPosition.y };
	sprHighScore->pos = sprHighScore->hidePosition;
	sprHighScore->size = { 302 * 0.666, 127 * 0.666 };






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
	if (!isResult)
	{
		// 全ウェーブ終了してるか
		if (Wave::Instance().state == 3)
		{
			isResult = true;

			// 値設定
			sprAliveTime->targetScore = SpinningTopPlayerManager::Instance().GetPlayer(0)->aliveTime;

			sprTotalDestoryEnemy->targetScore = SpinningTopPlayerManager::Instance().GetPlayer(0)->totalDestoryEnemy;
			sprGetTotalOption->targetScore = SpinningTopPlayerManager::Instance().GetPlayer(0)->getTotalOption;
			sprNowWave->targetScore = Wave::Instance().nowWave;
			sprTotalHitDamage->targetScore = SpinningTopPlayerManager::Instance().GetPlayer(0)->totalHitDamage;

			CalcScore();

			// ハイスコア更新
			if (sprTotalScore->targetScore > highScore)
			{
				highScore = sprTotalScore->targetScore;
				isHighScore = true;
			}
		}
		else
		{
			// プレイヤー死亡チェック
			if (SpinningTopPlayerManager::Instance().GetPlayerCount() <= 0) return;
			if (SpinningTopPlayerManager::Instance().GetPlayer(0)->isDead)
			{
				isResult = true;

				// 値設定
				sprAliveTime->targetScore = SpinningTopPlayerManager::Instance().GetPlayer(0)->aliveTime;

				sprTotalDestoryEnemy->targetScore = SpinningTopPlayerManager::Instance().GetPlayer(0)->totalDestoryEnemy;
				sprGetTotalOption->targetScore = SpinningTopPlayerManager::Instance().GetPlayer(0)->getTotalOption;
				sprNowWave->targetScore = Wave::Instance().nowWave;
				sprTotalHitDamage->targetScore = SpinningTopPlayerManager::Instance().GetPlayer(0)->totalHitDamage;

				CalcScore();

				// ハイスコア更新
				if (sprTotalScore->targetScore > highScore)
				{
					highScore = sprTotalScore->targetScore;
					isHighScore = true;
				}
			}
			else
			{
				return;
			}
		}
	}
	
	
	waitTimer += Timer::Instance().DeltaTime();
	if (waitTimer < waitTime) return;

	// 画面遷移
	if (waitTimer > resultFinishTime) resultFinish = true;
	if (InputManager::Instance().GetKeyReleased(Keyboard::Enter))
	{
		if(resultFinish)
			SceneManager::Instance().ChangeScene(new SceneSTMainMenu);
	}

	if (isResult)
	{
		sprMainMenu->showUi = true;
		sprSideBar1->showUi = true;
		sprSideBar2->showUi = true;

		sprRankS->showUi = true;
		sprRankA->showUi = true;
		sprRankB->showUi = true;
		sprRankC->showUi = true;

		sprTotalScore->showUi = true;
		sprAliveTime->showUi = true;

		sprHighScore->showUi = true;

		sprTotalDestoryEnemy->showUi = true;
		sprGetTotalOption->showUi = true;
		sprNowWave->showUi = true;
		sprTotalHitDamage->showUi = true;
	}

	sprMainMenu->UiUpdate();
	sprSideBar1->UiUpdate();
	sprSideBar2->UiUpdate();

	if (isHighScore)
	{
		if (waitTimer > startHightScore) sprHighScore->UiUpdate();
	}

	if (waitTimer > startRank)
	{
		if (sprTotalScore->targetScore > rankS)
		{
			sprRankS->UiUpdate();
		}
		else if (sprTotalScore->targetScore > rankA)
		{
			sprRankA->UiUpdate();
		}
		else if (sprTotalScore->targetScore > rankB)
		{
			sprRankB->UiUpdate();
		}
		else
		{
			sprRankC->UiUpdate();
		}
	}

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
	
	
	if (waitTimer > startRank)
	{
		if (sprTotalScore->targetScore > rankS)
		{
			sprRankS->UiRender();
		}
		else if (sprTotalScore->targetScore > rankA)
		{
			sprRankA->UiRender();
		}
		else if (sprTotalScore->targetScore > rankB)
		{
			sprRankB->UiRender();
		}
		else
		{
			sprRankC->UiRender();
		}
	}

	if (isHighScore)
	{
		if (waitTimer > startHightScore) sprHighScore->UiRender();
	}
}

void Result::CalcScore()
{
	float timeScore = sprAliveTime->targetScore * 100;
	float optionScore = sprGetTotalOption->targetScore * 500;
	float nowWaveScore = sprNowWave->targetScore * 1000;
	float hitDamageScore = -(sprTotalHitDamage->targetScore * 500);

	float totalDamageScore = 0;

	if (SpinningTopPlayerManager::Instance().GetPlayerCount() > 0)
	{
		StPlayer* pl = dynamic_cast<StPlayer*>(SpinningTopPlayerManager::Instance().GetPlayer(0));
		totalDamageScore = pl->totalHitScore;
	}

	sprTotalScore->targetScore = timeScore + optionScore + nowWaveScore + hitDamageScore + totalDamageScore;
}
