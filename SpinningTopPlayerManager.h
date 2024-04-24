#pragma once

#include <vector>
#include <set>
#include "StPlayer.h"

class SpinningTopPlayerManager {
private:
	SpinningTopPlayerManager() {}
	~SpinningTopPlayerManager() {}

public:
	// 唯一のインスタンス取得
	static SpinningTopPlayerManager& Instance() {
		static SpinningTopPlayerManager instance;
		return instance;
	}

	// 更新処理
	void Update();

	// 描画処理
	void Render();

	// プレイヤー登録
	void Register(StPlayerBase* player);

	// プレイヤー削除
	void Remove(StPlayerBase* player);

	// プレイヤー全削除
	void Clear();

	// プレイヤー数取得
	int GetPlayerCount() const { return static_cast<int>(players.size()); }

	// プレイヤー取得
	StPlayerBase* GetPlayer(int index) { return players.at(index); }

	// デバッグ用GUI描画
	void DrawDebugGui();

private:
	std::vector<StPlayerBase*> players;
	std::set<StPlayerBase*> removes;
};

