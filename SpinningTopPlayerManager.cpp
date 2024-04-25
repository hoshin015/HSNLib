#include "SpinningTopPlayerManager.h"

void SpinningTopPlayerManager::Update() {
	// 更新処理

	if (!adds.empty()) {
		players.insert(players.end(), adds.begin(), adds.end());
		adds.clear();
	}

	//当たり判定に入っている敵をリストに入れている
	StPlayerBase::UpdateEDistance(players);
	for (StPlayerBase* player : players) {
		player->Update();
	}

	// 破棄処理
	for (StPlayerBase* player : removes) {
		std::vector<StPlayerBase*>::iterator it = std::find(players.begin(), players.end(), player);

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
	for (StPlayerBase* player : players) {
		player->Render();
	}
}

// プレイヤー登録
void SpinningTopPlayerManager::Register(StPlayerBase* player) {
	adds.emplace_back(player);
	//players.emplace_back(player);
}


// プレイヤー削除
void SpinningTopPlayerManager::Remove(StPlayerBase* player) {
	// 破棄リストに追加
	removes.insert(player);
}

// プレイヤー全削除
void SpinningTopPlayerManager::Clear() {
	for (StPlayerBase* player : players) {
		delete player;
	}
	players.clear();
}

// デバッグ用GUI描画
void SpinningTopPlayerManager::DrawDebugGui() {
	for (StPlayerBase* player : players) {
		player->DrawDebugGui();
	}
}
