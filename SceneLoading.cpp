#include "SceneLoading.h"
#include "SceneManager.h"
#include "Library/Input/InputManager.h"
#include "Library/Timer.h"

void SceneLoading::Initialize()
{
	sprite = new Sprite(L"Data/Texture/Pause/select.png");

	// �X���b�h�J�n
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

	// ���̃V�[���̏������ł�����V�[����؂�ւ���
	if(nextScene->IsReady())
	{
		SceneManager::Instance().ChangeScene(nextScene);
	}
}

void SceneLoading::Render()
{
	sprite->Render(1144, 585, 86, 85, 1, 1, 1, 1, angle);
}

// ���[�f�B���O�X���b�h
void SceneLoading::LoadingThread(SceneLoading* scene)
{
	// COM�֘A�̏������ŃX���b�h���ɌĂԕK�v������
	CoInitialize(nullptr);

	// ���̃V�[���̏��������s��
	scene->nextScene->Initialize();

	// �X���b�h���I���O��COM�֘A�̏I����
	CoUninitialize();

	// ���̃V�[���̏��������ݒ�
	scene->nextScene->SetReady();
}
