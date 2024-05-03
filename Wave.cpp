#include "Wave.h"
#include "Library/Timer.h"

#include "WaveData_1.h"

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
}

void Wave::Update()
{
	switch (state)
	{
	case 0:	// ウェーブ初期化	
	{
		pScript = *ppScript;
		timer = 0.0f;
		state++;
		break;
	}
	case 1:	// ウェーブ進行
	{
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
			state = *ppScript ? 0 : 2;
		}

		break;
	}
	case 2:	// 全ウェーブ終了
	{
		break;
	}
	}
}
