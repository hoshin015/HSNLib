#include "Wave.h"
#include "Library/Timer.h"

#include "WaveData_1.h"
#include "SpinningTopPlayerManager.h"
#include "Library/Easing.h"

// ウェーブデータ
WaveScript* waveScript[] =
{
	wave1Script,
	wave2Script,
	wave3Script,
	wave4Script,
	//wave5Script,
	//wave6Script,
	//wave7Script,
	//wave8Script,
	//wave9Script,
	//wave10Script,
	nullptr,
};


void Wave::Init()
{
	state = 0;

	ppScript = waveScript;

	sprWaveLine = std::make_unique<Sprite>(L"Data/Texture/Wave/WaveLine.png");
	sprWaveCut1 = std::make_unique<Sprite>(L"Data/Texture/Wave/WaveCut1.png");
	sprWaveCut2 = std::make_unique<Sprite>(L"Data/Texture/Wave/WaveCut2.png");
	sprWaveCut3 = std::make_unique<Sprite>(L"Data/Texture/Wave/WaveCut3.png");
	sprWaveCut4 = std::make_unique<Sprite>(L"Data/Texture/Wave/WaveCut4.png");

	nowWave = 0;
}

void Wave::Update()
{
	switch (state)
	{
	case 0:	// ウェーブ初期化	
	{
		pScript = *ppScript;
		timer = 0.0f;
		waveLimitTimer = waveLimitTime;
		nowWave++;
		showWaveState = WaveShowState::showWaveInit;

		// プレイヤー
		if (SpinningTopPlayerManager::Instance().GetPlayerCount() > 0)
		{
			StPlayer* pl = dynamic_cast<StPlayer*>(SpinningTopPlayerManager::Instance().GetPlayer(0));
			if (pl)
			{
				while (pl->option.size())
				{
					pl->EraseOption();
				}

				pl->partsCount = 0;
			}
		}

		state++;
		break;
	}
	case 1:	// ウェーブ表示
	{
		showWaveTextTimer += Timer::Instance().DeltaTime();

		switch (showWaveState)
		{
		case Wave::showWaveInit:
			cutCurrentPos = startPos;
			showWaveTextTimer = 0.0f;
			showWaveState++;
			[[fallthrough]];
		case Wave::goCenter:
			cutCurrentPos.x = startPos.x + (centerStartPos.x - startPos.x) * Easing(showWaveTextTimer, showWaveTextEaseTime, easeNormal, easeIn);
			if (showWaveTextTimer > showWaveTextEaseTime)
			{
				showWaveTextTimer = 0.0f;
				showWaveState++;
			}
			break;
		case Wave::waitCenter:
			cutCurrentPos.x = centerStartPos.x + (centerEndPos.x - centerStartPos.x) * Easing(showWaveTextTimer, showWaveTextWaitTime, easeNormal, easeIn);
			if (showWaveTextTimer > showWaveTextWaitTime)
			{
				showWaveTextTimer = 0.0f;
				showWaveState++;
			}
			break;
		case Wave::goOutSide:
			cutCurrentPos.x = centerEndPos.x + (endPos.x - centerEndPos.x) * Easing(showWaveTextTimer, showWaveTextEaseTime, easeNormal, easeIn);
			if (showWaveTextTimer > showWaveTextEaseTime)
			{
				showWaveTextTimer = 0.0f;
				showWaveState++;
			}
			break;
		case Wave::showWaveEnd:
			state++;
			break;
		default:
			break;
		}

		break;
	}
	case 2:	// ウェーブ進行
	{
		// プレイヤー死亡チェック
		if (SpinningTopPlayerManager::Instance().GetPlayer(0)->isDead)
		{
			state = 4;
			break;
		}
		SpinningTopPlayerManager::Instance().GetPlayer(0)->aliveTime += Timer::Instance().DeltaTime();

		waveLimitTimer -= Timer::Instance().DeltaTime();
		timer += Timer::Instance().DeltaTime();
		while (pScript->time <= timer)
		{
			if (pScript->data == nullptr)
			{
				break;
			}
			pScript->data->Execute();

			pScript++;
		}

		// ウェーブクリアチェック(全部のデータを出し切ってnullptrにたどりついたら次のウェーブへ)
		if (pScript->data == nullptr)
		{
			ppScript++;
			state = *ppScript ? 0 : 3;
		}

		break;
	}
	case 3:	// 全ウェーブ終了
	{
		break;
	}
	case 4:	// プレイヤー死亡
	{	
		break;
	}
	}
}

// 描画処理
void Wave::Render()
{
	if (state == 1)
	{
		sprWaveLine->Render(0, 296, 1280, 128, 1, 1, 1, 1, 0);

		switch (nowWave)
		{
		case 1: sprWaveCut1->Render(cutCurrentPos.x, cutCurrentPos.y, 256, 128, 1, 1, 1, 1, 0);	break;
		case 2: sprWaveCut2->Render(cutCurrentPos.x, cutCurrentPos.y, 256, 128, 1, 1, 1, 1, 0);	break;
		case 3: sprWaveCut3->Render(cutCurrentPos.x, cutCurrentPos.y, 256, 128, 1, 1, 1, 1, 0);	break;
		case 4: sprWaveCut4->Render(cutCurrentPos.x, cutCurrentPos.y, 256, 128, 1, 1, 1, 1, 0);	break;
		default:
			break;
		}
		
	}
	
}
