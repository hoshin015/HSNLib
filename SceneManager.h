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

// シーンマネージャー
class SceneManager
{
private:
	SceneManager() {}
	~SceneManager() {}

public:
	// 唯一のインスタンス取得
	static SceneManager& Instance()
	{
		static SceneManager instance;
		return instance;
	}

	// 更新処理
	void Update();

	// 描画処理
	void Render();

	// シーンクリア
	void Clear();

	// シーン切り替え
	void ChangeScene(Scene* scene);

private:
	Scene* currentScene = nullptr;
	Scene* nextScene = nullptr;
};