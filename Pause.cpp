#include "Pause.h"
#include "Library/Input/InputManager.h"
#include "Library/Graphics/Graphics.h"
#include "Library/Timer.h"
#include "SceneManager.h"

Pause::Pause()
{
	// --- ポーズ画面 ---
	{
		float deltaRightX = 1300;
		float deltaLeftX = -1300;

		sprGiveUp = std::make_unique<UiSprite>(L"Data/Texture/Pause/giveUp.png");
		sprGiveUp->showPosition = { 100, 550 };
		sprGiveUp->hidePosition = { sprGiveUp->showPosition.x + deltaRightX, sprGiveUp->showPosition.y };
		sprGiveUp->pos = sprGiveUp->hidePosition;
		sprGiveUp->size = { 365 * 0.666, 133 * 0.666 };


		sprGameRestart = std::make_unique<UiSprite>(L"Data/Texture/Pause/gameRestart.png");
		sprGameRestart->showPosition = { 1000, 50 };
		sprGameRestart->hidePosition = { sprGameRestart->showPosition.x + deltaLeftX, sprGameRestart->showPosition.y };
		sprGameRestart->pos = sprGameRestart->hidePosition;
		sprGameRestart->size = { 365 * 0.666, 133 * 0.666 };


		sprRetry = std::make_unique<UiSprite>(L"Data/Texture/Pause/retry.png");
		sprRetry->showPosition = { 700, 150 };
		sprRetry->hidePosition = { sprRetry->showPosition.x + deltaRightX, sprRetry->showPosition.y };
		sprRetry->pos = sprRetry->hidePosition;
		sprRetry->size = { 631 * 0.666, 517 * 0.666 };


		sprSideBar1 = std::make_unique<UiSprite>(L"Data/Texture/Pause/sideBar1.png");
		sprSideBar1->showPosition = { 0,0 };
		sprSideBar1->hidePosition = { sprSideBar1->showPosition.y + deltaLeftX, sprSideBar1->showPosition.y };
		sprSideBar1->pos = sprSideBar1->hidePosition;
		sprSideBar1->size = { 1294 * 0.666, 794 * 0.666 };

		sprSideBar2 = std::make_unique<UiSprite>(L"Data/Texture/Pause/sideBar2.png");
		sprSideBar2->showPosition = { 930,328 };
		sprSideBar2->hidePosition = { sprSideBar2->showPosition.x + deltaRightX, sprSideBar2->showPosition.y };
		sprSideBar2->pos = sprSideBar2->hidePosition;
		sprSideBar2->size = { 526 * 0.666, 589 * 0.666 };

		sprSelect0 = std::make_unique<UiSprite>(L"Data/Texture/Pause/select.png");
		sprSelect0->showPosition = { 950, 50 };
		sprSelect0->hidePosition = { sprSelect0->showPosition.x + deltaLeftX, sprSelect0->showPosition.y };
		sprSelect0->pos = sprSelect0->hidePosition;
		sprSelect0->size = { 82, 81 };

		sprSelect1 = std::make_unique<UiSprite>(L"Data/Texture/Pause/select.png");
		sprSelect1->showPosition = { 700, 150 };
		sprSelect1->hidePosition = { sprSelect1->showPosition.x + deltaRightX, sprSelect1->showPosition.y };
		sprSelect1->pos = sprSelect1->hidePosition;
		sprSelect1->size = { 82, 81 };

		sprSelect2 = std::make_unique<UiSprite>(L"Data/Texture/Pause/select.png");
		sprSelect2->showPosition = { 310, 550 };
		sprSelect2->hidePosition = { sprSelect2->showPosition.x + deltaRightX, sprSelect2->showPosition.y };
		sprSelect2->pos = sprSelect2->hidePosition;
		sprSelect2->size = { 82, 81 };
	}

}

void Pause::Update()
{
	InputManager& input = InputManager::Instance();
	bool up, down;
	up = down = false;
	XMVECTOR LSVec = XMVectorSet(
		input.GetThumSticksLeftX()
		- input.GetGamePadButtonPressed(GAMEPADBUTTON_STATE::dpadLeft) + input.GetGamePadButtonPressed(GAMEPADBUTTON_STATE::dpadRight),

		input.GetThumSticksLeftY()
		+ input.GetGamePadButtonPressed(GAMEPADBUTTON_STATE::dpadUp) - input.GetGamePadButtonPressed(GAMEPADBUTTON_STATE::dpadDown),
		0, 0);

	XMVECTOR upVec = XMVector2Normalize(XMVectorSet(1, 1, 0, 0));
	float dot = XMVectorGetX(XMVector2Dot(LSVec, upVec));
	float deadZone = 0.5f;

	static bool before = false;
	bool state = false;
	if (XMVectorGetX(XMVector2Length(LSVec)) < deadZone)state = true;
	(dot > 0 ? up : down) = (before ^ state) && before;
	before = state;

	if (input.GetKeyPressed(DirectX::Keyboard::Space) || input.GetGamePadButtonPressed(GAMEPADBUTTON_STATE::start))
	{
		if (isPause)
		{
			isPause = false;
			sprGiveUp->showUi = false;
			sprGameRestart->showUi = false;
			sprRetry->showUi = false;
			sprSideBar1->showUi = false;
			sprSideBar2->showUi = false; 
			sprSelect0->showUi = false;
			sprSelect1->showUi = false;
			sprSelect2->showUi = false;
		}
		else
		{
			isPause = true;
			sprGiveUp->showUi = true;
			sprGameRestart->showUi = true;
			sprRetry->showUi = true;
			sprSideBar1->showUi = true;
			sprSideBar2->showUi = true;
			sprSelect0->showUi = true;
			sprSelect1->showUi = true;
			sprSelect2->showUi = true;

			selectNum = 0;
		}
	}

	// puaseUi更新
	{
		sprGiveUp->UiUpdate();
		sprGameRestart->UiUpdate();
		sprRetry->UiUpdate();
		sprSideBar1->UiUpdate();
		sprSideBar2->UiUpdate();
		sprSelect0->UiUpdate();
		sprSelect1->UiUpdate();
		sprSelect2->UiUpdate();

		sprSelect0->angle += 120 * Timer::Instance().DeltaTime();
		sprSelect1->angle += 120 * Timer::Instance().DeltaTime();
		sprSelect2->angle += 120 * Timer::Instance().DeltaTime();
	}

	if (isPause)
	{
		if (input.GetKeyPressed(DirectX::Keyboard::W) || up)
		{
			selectNum--;
		}
		if (input.GetKeyPressed(DirectX::Keyboard::S) || down)
		{
			selectNum++;
		}
		selectNum = (selectNum + static_cast<int>(selectMenu::END)) % static_cast<int>(selectMenu::END);

		if (input.GetKeyReleased(DirectX::Keyboard::Enter) || input.GetGamePadButtonReleased(GAMEPADBUTTON_STATE::a))
		{
			switch (selectNum)
			{
			case static_cast<int>(selectMenu::RESTART):
			{
				isPause = false;
				sprGiveUp->showUi = false;
				sprGameRestart->showUi = false;
				sprRetry->showUi = false;
				sprSideBar1->showUi = false;
				sprSideBar2->showUi = false;
				sprSelect0->showUi = false;
				sprSelect1->showUi = false;
				sprSelect2->showUi = false;
				break;
			}
			case static_cast<int>(selectMenu::RETRY):
			{
				if (dynamic_cast<SceneWave*>(SceneManager::Instance().GetCurrentScene()))
					SceneManager::Instance().ChangeScene(new SceneLoading(new SceneWave));
				if (dynamic_cast<SceneSTTutorial*>(SceneManager::Instance().GetCurrentScene()))
					SceneManager::Instance().ChangeScene(new SceneLoading(new SceneSTTutorial));
				break;
			}
			case static_cast<int>(selectMenu::GIVEUP):
			{
				SceneManager::Instance().ChangeScene(new SceneSTTitle);
				break;
			}
			}
		}
	}
}

void Pause::Render()
{
	Graphics* gfx = &Graphics::Instance();
	gfx->SetBlend(BLEND_STATE::ALPHA);
	sprGiveUp->UiRender();
	sprGameRestart->UiRender();
	sprRetry->UiRender();
	sprSideBar1->UiRender();
	sprSideBar2->UiRender();
	

	if (isPause)
	{
		switch (selectNum)
		{
		case static_cast<int>(selectMenu::RESTART):
		{
			sprSelect0->UiRender();
			break;
		}
		case static_cast<int>(selectMenu::RETRY):
		{
			sprSelect1->UiRender();
			break;
		}
		case static_cast<int>(selectMenu::GIVEUP):
		{
			sprSelect2->UiRender();
			break;
		}
		}
	}
	
}
