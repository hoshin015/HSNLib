#include "Wave.h"
#include "Library/Timer.h"

#include "WaveData_1.h"
#include "SpinningTopPlayerManager.h"

// ウェーブデータ
WaveScript* waveScript[] =
{
	wave1Script,
	wave2Script,
	wave3Script,
	wave4Script,
	wave5Script,
	wave6Script,
	wave7Script,
	wave8Script,
	wave9Script,
	wave10Script,
	nullptr,
};


void Wave::Init()
{
	state = 0;

	ppScript = waveScript;

	sprWaveCut1 = std::make_unique<Sprite>(L"Data/Texture/Wave/WaveCut1.png");
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
		state++;
		break;
	}
	case 1:	// ウェーブ表示
	{


		state++;
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
