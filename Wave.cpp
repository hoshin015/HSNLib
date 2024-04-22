#include "Wave.h"
#include "Library/Timer.h"

void Wave::Init()
{
	state = 0;
}

void Wave::Update()
{
	switch (state)
	{
	case 0:
	{
		pScript = *ppScript;
		timer = 0.0f;
		state++;
		break;
	}
	case 1:
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
	}
	default:
		break;
	}
}
