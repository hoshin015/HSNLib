#include "AudioManager.h"
#include "../ErrorLogger.h"

// èâä˙âªèàóù
void AudioManager::Initialize()
{
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	// ÉtÉâÉOê∂ê¨
	DirectX::AUDIO_ENGINE_FLAGS eflags = DirectX::AudioEngine_Default;

#if _DEBUG
	eflags |= DirectX::AudioEngine_Debug;
#endif

	// audioEngine ê∂ê¨
	audioEngine = std::make_unique<DirectX::AudioEngine>(eflags);


	//LoadMusic(static_cast<int>(MUSIC_LABEL::WEAPON), L"Data/Audio/Impact.wav");

	//SEÇÃÇ›ì«Ç›çûÇﬁ
	LoadMusic(PAUSE_CONFIRM,      L"Data/Audio/SE/Pause/Confirm.wav");
	LoadMusic(PAUSE_SELECT,       L"Data/Audio/SE/Pause/Select.wav");
	LoadMusic(PAUSE_SHOW,         L"Data/Audio/SE/Pause/Show.wav");
	LoadMusic(MAIN_CONFIRM,       L"Data/Audio/SE/MainMenu/Confirm.wav");
	LoadMusic(MAIN_SELECT,        L"Data/Audio/SE/MainMenu/Select.wav");
	LoadMusic(PLAYER_BREAK,       L"Data/Audio/SE/Player/Break.wav");
	LoadMusic(PLAYER_HITPARRY,    L"Data/Audio/SE/Player/HitParry.wav");
	LoadMusic(PLAYER_INVAINPARRY, L"Data/Audio/SE/Player/InVainParry.wav");
	LoadMusic(PLAYER_BODYBLOW,    L"Data/Audio/SE/Player/BodyBlow.wav");
	LoadMusic(PLAYER_GETOP,       L"Data/Audio/SE/Player/GetOption.wav");
}

