#include "SpinningTopPlayerManager.h"

void SpinningTopPlayerManager::Update() {
	// �X�V����
	for (StPlayer* player : players) {
		player->Update();
	}

	// �j������
	for (StPlayer* player : removes) {
		std::vector<StPlayer*>::iterator it = std::find(players.begin(), players.end(), player);

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
	for (StPlayer* player : players) {
		player->Render();
	}
}

// �v���C���[�o�^
void SpinningTopPlayerManager::Register(StPlayer* player) {
	players.emplace_back(player);
}


// �v���C���[�폜
void SpinningTopPlayerManager::Remove(StPlayer* player) {
	// �j�����X�g�ɒǉ�
	removes.insert(player);
}

// �v���C���[�S�폜
void SpinningTopPlayerManager::Clear() {
	for (StPlayer* player : players) {
		delete player;
	}
	players.clear();
}

// �f�o�b�O�pGUI�`��
void SpinningTopPlayerManager::DrawDebugGui() {
	for (StPlayer* player : players) {
		player->DrawDebugGui();
	}
}
