#include "SceneLoading.h"
#include "SceneManager.h"
#include "Library/Input/InputManager.h"
#include "Library/Timer.h"

void SceneLoading::Initialize()
{
	sprite = new Sprite(L"Data/Texture/Pause/select.png");

	// スレッド開始
	thread = new std::thread(LoadingThread, this);
}

void SceneLoading::Finalize()
{
	delete sprite;
	if (thread->joinable()) {
		thread->join();
	}
	delete thread;
}

void SceneLoading::Update()
{
	constexpr float speed = 180;
	angle += speed * Timer::Instance().DeltaTime();

	// 次のシーンの準備ができたらシーンを切り替える
	if(nextScene->IsReady())
	{
		SceneManager::Instance().ChangeScene(nextScene);
	}
}

void SceneLoading::Render()
{
	sprite->Render(1144, 585, 86, 85, 1, 1, 1, 1, angle);
}

// ローディングスレッド
void SceneLoading::LoadingThread(SceneLoading* scene)
{
	// COM関連の初期化でスレッド毎に呼ぶ必要がある
	CoInitialize(nullptr);

	// 次のシーンの初期化を行う
	scene->nextScene->Initialize();

	// スレッドが終わる前にCOM関連の終了化
	CoUninitialize();

	// 次のシーンの準備完了設定
	scene->nextScene->SetReady();
}
