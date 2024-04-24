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
	void Register(StPlayerBase* player);

	// �v���C���[�폜
	void Remove(StPlayerBase* player);

	// �v���C���[�S�폜
	void Clear();

	// �v���C���[���擾
	int GetPlayerCount() const { return static_cast<int>(players.size()); }

	// �v���C���[�擾
	StPlayerBase* GetPlayer(int index) { return players.at(index); }

	// �f�o�b�O�pGUI�`��
	void DrawDebugGui();

private:
	std::vector<StPlayerBase*> players;
	std::set<StPlayerBase*> removes;
};

