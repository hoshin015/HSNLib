#pragma once

#include <vector>
#include <set>
#include "Player.h"

// �v���C���[�}�l�[�W���[
class PlayerManager
{
private:
	PlayerManager() {}
	~PlayerManager() {}

public:
	// �B��̃C���X�^���X�擾
	static PlayerManager& Instance()
	{
		static PlayerManager instance;
		return instance;
	}

	// �X�V����
	void Update();

	// �`�揈��
	void Render();
	
	// �v���C���[�o�^
	void Register(Player* player);

	// �v���C���[�폜
	void Remove(Player* player);

	// �v���C���[�S�폜
	void Clear();

	// �v���C���[���擾
	int GetPlayerCount() const { return static_cast<int>(players.size()); }

	// �v���C���[�擾
	Player* GetPlayer(int index) { return players.at(index); }

	// �f�o�b�O�pGUI�`��
	void DrawDebugGui();

private:
	std::vector<Player*> players;
	std::set<Player*> removes;
};