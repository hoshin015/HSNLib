#include "Wave.h"
#include "Library/Timer.h"

#include "WaveData_1.h"

// �E�F�[�u�f�[�^
WaveScript* waveScript[] = {
	wave1Script, // �X�e�[�W1
	nullptr,      // �I���R�[�h
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
		break;
	}
	case 2:	// �S�E�F�[�u�I��
	{
		break;
	}
	}
}
