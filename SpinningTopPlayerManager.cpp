#include "SpinningTopPlayerManager.h"

void SpinningTopPlayerManager::Update() {
	// XV
	for (StPlayerBase* player : players) {
		if (player->IsPlayer()) {
			auto option = std::find_if(players.begin(), players.end(), [](StPlayerBase* option) {return !option->IsPlayer(); });

		}
		player->Update();
	}

	// jü
	for (StPlayerBase* player : removes) {
		std::vector<StPlayerBase*>::iterator it = std::find(players.begin(), players.end(), player);

		if (it != players.end()) {
			players.erase(it);
		}
		// eÛÌjü
		delete player;
	}
	// jüXgðNA
	removes.clear();
}

//@`æ
void SpinningTopPlayerManager::Render() {
	for (StPlayerBase* player : players) {
		player->Render();
	}
}

// vC[o^
void SpinningTopPlayerManager::Register(StPlayerBase* player) {
	players.emplace_back(player);
}


// vC[í
void SpinningTopPlayerManager::Remove(StPlayerBase* player) {
	// jüXgÉÇÁ
	removes.insert(player);
}

// vC[Sí
void SpinningTopPlayerManager::Clear() {
	for (StPlayerBase* player : players) {
		delete player;
	}
	players.clear();
}

// fobOpGUI`æ
void SpinningTopPlayerManager::DrawDebugGui() {
	for (StPlayerBase* player : players) {
		player->DrawDebugGui();
	}
}
