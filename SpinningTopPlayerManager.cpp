#include "SpinningTopPlayerManager.h"

void SpinningTopPlayerManager::Update() {
	// �X�V����

	if (!adds.empty()) {
		players.insert(players.end(), adds.begin(), adds.end());
		adds.clear();
	}

	//�����蔻��ɓ����Ă���G�����X�g�ɓ���Ă���
	StPlayerBase::UpdateEDistance(players);
	for (StPlayerBase* player : players) {
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
	adds.emplace_back(player);
	//players.emplace_back(player);
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
