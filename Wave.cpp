#include "Wave.h"
#include "Library/Timer.h"

#include "WaveData_1.h"

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
}

void Wave::Update()
{
	switch (state)
	{
	case 0:	// �E�F�[�u������	
	{
		pScript = *ppScript;
		timer = 0.0f;
		state++;
		break;
	}
	case 1:	// �E�F�[�u�i�s
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

		// �E�F�[�u�N���A�`�F�b�N(�S���̃f�[�^���o���؂���nullptr�ɂ��ǂ�����玟�̃E�F�[�u��)
		if (pScript->data == nullptr)
		{
			ppScript++;
			state = *ppScript ? 0 : 2;
		}

		break;
	}
	case 2:	// �S�E�F�[�u�I��
	{
		break;
	}
	}
}
