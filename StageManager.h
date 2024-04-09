#pragma once

#include <vector>
#include "Stage.h"

// ステージマネージャー
class StageManager
{
private:
	StageManager() {}
	~StageManager() {}

public:
	// 唯一のインスタンス取得
	static StageManager& Instance()
	{
		static StageManager instance;
		return instance;
	}

	// 更新処理
	void Update();

	// 描画処理
	void Render();

	// ステージ登録
	void Register(Stage* stage);

	// ステージ全削除
	void Clear();

	// レイキャスト
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit, int id);

private:
	std::vector<Stage*> stages;
};