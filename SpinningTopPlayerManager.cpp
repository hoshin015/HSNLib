#include "SpinningTopPlayerManager.h"

void SpinningTopPlayerManager::Update() {
	// �X�V����
	for (StPlayerBase* player : players) {
		if (player->IsPlayer()) {
			auto option = std::find_if(players.begin(), players.end(), [](StPlayerBase* option) {return !option->IsPlayer(); });

		}
		player->Update();
	}

	// �j������
	for (StPlayerBase* player : removes) {
		std::vector<StPlayerBase*>::iterator it = std::find(players.begin(), players.end(), player);

		if (it != players.end()) {
			players.erase(it);
		}
		// �e�ۂ̔j������
		delete player;
	}
	// �j�����X�g���N���A
	removes.clear();
}

//�@�`�揈��
void SpinningTopPlayerManager::Render() {
	for (StPlayerBase* player : players) {
		player->Render();
	}
}

// �v���C���[�o�^
void SpinningTopPlayerManager::Register(StPlayerBase* player) {
	players.emplace_back(player);
}


// �v���C���[�폜
void SpinningTopPlayerManager::Remove(StPlayerBase* player) {
	// �j�����X�g�ɒǉ�
	removes.insert(player);
}

// �v���C���[�S�폜
void SpinningTopPlayerManager::Clear() {
	for (StPlayerBase* player : players) {
		delete player;
	}
	players.clear();
}

// �f�o�b�O�pGUI�`��
void SpinningTopPlayerManager::DrawDebugGui() {
	for (StPlayerBase* player : players) {
		player->DrawDebugGui();
	}
}
