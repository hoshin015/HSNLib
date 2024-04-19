#pragma once

#include <vector>
#include <set>
#include "StPlayer.h"

class SpinningTopPlayerManager {
private:
	SpinningTopPlayerManager() {}
	~SpinningTopPlayerManager() {}

public:
	// �B��̃C���X�^���X�擾
	static SpinningTopPlayerManager& Instance() {
		static SpinningTopPlayerManager instance;
		return instance;
	}

	// �X�V����
	void Update();

	// �`�揈��
	void Render();

	// �v���C���[�o�^
	void Register(StPlayer* player);

	// �v���C���[�폜
	void Remove(StPlayer* player);

	// �v���C���[�S�폜
	void Clear();

	// �v���C���[���擾
	int GetPlayerCount() const { return static_cast<int>(players.size()); }

	// �v���C���[�擾
	StPlayer* GetPlayer(int index) { return players.at(index); }

	// �f�o�b�O�pGUI�`��
	void DrawDebugGui();

private:
	std::vector<StPlayer*> players;
	std::set<StPlayer*> removes;
};

