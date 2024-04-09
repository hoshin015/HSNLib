#pragma once

#include <vector>
#include <set>
#include "Player.h"

// プレイヤーマネージャー
class PlayerManager
{
private:
	PlayerManager() {}
	~PlayerManager() {}

public:
	// 唯一のインスタンス取得
	static PlayerManager& Instance()
	{
		static PlayerManager instance;
		return instance;
	}

	// 更新処理
	void Update();

	// 描画処理
	void Render();
	
	// プレイヤー登録
	void Register(Player* player);

	// プレイヤー削除
	void Remove(Player* player);

	// プレイヤー全削除
	void Clear();

	// プレイヤー数取得
	int GetPlayerCount() const { return static_cast<int>(players.size()); }

	// プレイヤー取得
	Player* GetPlayer(int index) { return players.at(index); }

	// デバッグ用GUI描画
	void DrawDebugGui();

private:
	std::vector<Player*> players;
	std::set<Player*> removes;
};