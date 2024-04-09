#pragma once
#include <Windows.h>
#include <Audio.h>


enum class MUSIC_LABEL
{
	BGM_TEST = 0,
	WEAPON,
};

class AudioManager
{
private:
	AudioManager() {}
	~AudioManager() {}

public:
	static AudioManager& Instance()
	{
		static AudioManager instance;
		return instance;
	}

private:
	// AudioEngine
	std::unique_ptr<DirectX::AudioEngine> audioEngine;

	static const int MUSIC_FILE_MAX = 32;
	static const int XWB_FILE_MAX = 16;

	// Music
	struct Music
	{
		std::unique_ptr<DirectX::SoundEffect>			music = nullptr;
		std::unique_ptr<DirectX::SoundEffectInstance>	musicInst = nullptr;
		float											volume = 1.0f;
	} musicData[MUSIC_FILE_MAX];

public:
	// 初期化処理
	void Initialize();

	// 音楽の読み込み
	void LoadMusic(int trackNo, const wchar_t* waveFilename, float volume = 1.0f)
	{
		if (musicData[trackNo].musicInst) musicData[trackNo].musicInst.reset();
		musicData[trackNo].music.reset(new DirectX::SoundEffect(audioEngine.get(), waveFilename));
		musicData[trackNo].volume = volume;
	}

	// 音楽の解放
	void UnLoadMusic(int trackNo)
	{
		if (musicData[trackNo].musicInst)  musicData[trackNo].musicInst.reset();
		if (musicData[trackNo].music)  musicData[trackNo].music.reset(nullptr);
	}

	// 音楽再生
	void PlayMusic(int trackNo, bool isLoop = false)
	{
		if (!musicData[trackNo].music) return;
		musicData[trackNo].musicInst = musicData[trackNo].music->CreateInstance();
		musicData[trackNo].musicInst->Play(isLoop);
		musicData[trackNo].musicInst->SetVolume(musicData[trackNo].volume);
	}

	// 音楽停止
	void StopMusic(int trackNo)
	{
		if (!musicData[trackNo].musicInst) return;
		musicData[trackNo].musicInst->Stop();
	}

	// 音楽ポーズ
	void PauseMusic(int trackNo)
	{
		if (!musicData[trackNo].musicInst) return;
		musicData[trackNo].musicInst->Pause();
	}

	// 音楽レジューム
	void ResumeMusic(int trackNo)
	{
		if (!musicData[trackNo].musicInst) return;
		musicData[trackNo].musicInst->Resume();
	}

	// 音楽ボリューム設定
	void SetMusicVolume(int trackNo, int volume)
	{
		if (!musicData[trackNo].musicInst) return;
		musicData[trackNo].musicInst->SetVolume(volume);
		musicData[trackNo].volume = volume;
	}

	// 音楽状態取得
	DirectX::SoundState GetSoundState(int trackNo)
	{
		return musicData[trackNo].musicInst->GetState();
	}

	// 音楽ループ状態取得
	bool IsLoopMusic(int trackNo)
	{
		return musicData[trackNo].musicInst->IsLooped();
	}

	// 音楽使用中かどうか
	bool IsInUseMusic(int trackNo)
	{
		return musicData[trackNo].music->IsInUse();
	}
};