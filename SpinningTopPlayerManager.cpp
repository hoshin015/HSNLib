#include "SpinningTopPlayerManager.h"

void SpinningTopPlayerManager::Update() {
	// 更新処理
	for (StPlayer* player : players) {
		player->Update();
	}

	// 破棄処理
	for (StPlayer* player : removes) {
		std::vector<StPlayer*>::iterator it = std::find(players.begin(), players.end(), player);

		if (it != players.end()) {
			players.erase(it);
		}
		// 弾丸の破棄処理
		delete player;
	}
	// 破棄リストをクリア
	removes.clear();
}

//　描画処理
void SpinningTopPlayerManager::Render() {
	for (StPlayer* player : players) {
		player->Render();
	}
}

// プレイヤー登録
void SpinningTopPlayerManager::Register(StPlayer* player) {
	players.emplace_back(player);
}


// プレイヤー削除
void SpinningTopPlayerManager::Remove(StPlayer* player) {
	// 破棄リストに追加
	removes.insert(player);
}

// プレイヤー全削除
void SpinningTopPlayerManager::Clear() {
	for (StPlayer* player : players) {
		delete player;
	}
	players.clear();
}

// デバッグ用GUI描画
void SpinningTopPlayerManager::DrawDebugGui() {
	for (StPlayer* player : players) {
		player->DrawDebugGui();
	}
}
