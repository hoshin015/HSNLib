#pragma once
#include "Scene.h"
#include "SceneGame.h"
#include "SceneTitle.h"
#include "SceneLoading.h"
#include "SceneAnimation.h"
#include "SceneContextBase.h"
#include "SceneTest.h"
#include "SceneSpinningTop.h"
#include "SceneSTPlayer.h"
#include "SceneEnemyLevelEditor.h"
#include "Library/MemoryLeak.h"

// �V�[���}�l�[�W���[
class SceneManager
{
private:
	SceneManager() {}
	~SceneManager() {}

public:
	// �B��̃C���X�^���X�擾
	static SceneManager& Instance()
	{
		static SceneManager instance;
		return instance;
	}

	// �X�V����
	void Update();

	// �`�揈��
	void Render();

	// �V�[���N���A
	void Clear();

	// �V�[���؂�ւ�
	void ChangeScene(Scene* scene);

private:
	Scene* currentScene = nullptr;
	Scene* nextScene = nullptr;
};