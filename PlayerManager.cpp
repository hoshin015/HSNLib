#include "PlayerManager.h"

// 更新処理
void PlayerManager::Update()
{
	// 更新処理
	for (Player* player : players)
	{
		player->Update();
	}

	// 破棄処理
	for (Player* player : removes)
	{
		std::vector<Player*>::iterator it = std::find(players.begin(), players.end(), player);

		if (it != players.end())
		{
			players.erase(it);
		}
		// 弾丸の破棄処理
		delete player;
	}
	// 破棄リストをクリア
	removes.clear();
}

//　描画処理
void PlayerManager::Render()
{
	for (Player* player : players)
	{
		player->Render();
	}
}

// プレイヤー登録
void PlayerManager::Register(Player* player)
{
	players.emplace_back(player);
}


// プレイヤー削除
void PlayerManager::Remove(Player* player)
{
	// 破棄リストに追加
	removes.insert(player);
}

// プレイヤー全削除
void PlayerManager::Clear()
{
	for (Player* player : players)
	{
		delete player;
	}
	players.clear();
}

// デバッグ用GUI描画
void PlayerManager::DrawDebugGui()
{
	for (Player* player : players)
	{
		player->DrawDebugGui();
	}
}
