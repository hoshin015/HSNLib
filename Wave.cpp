#include "Wave.h"
#include "Library/Timer.h"

#include "WaveData_1.h"
#include "SpinningTopPlayerManager.h"

// �E�F�[�u�f�[�^
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
	case 0:	// �E�F�[�u������	
	{
		pScript = *ppScript;
		timer = 0.0f;
		waveLimitTimer = waveLimitTime;
		nowWave++;
		state++;
		break;
	}
	case 1:	// �E�F�[�u�\��
	{


		state++;
		break;
	}
	case 2:	// �E�F�[�u�i�s
	{
		// �v���C���[���S�`�F�b�N
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

		// �E�F�[�u�N���A�`�F�b�N(�S���̃f�[�^���o���؂���nullptr�ɂ��ǂ�����玟�̃E�F�[�u��)
		if (pScript->data == nullptr)
		{
			ppScript++;
			state = *ppScript ? 0 : 3;
		}

		break;
	}
	case 3:	// �S�E�F�[�u�I��
	{
		break;
	}
	case 4:	// �v���C���[���S
	{	
		break;
	}
	}
}
