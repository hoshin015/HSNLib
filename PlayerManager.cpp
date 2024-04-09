#include "PlayerManager.h"

// �X�V����
void PlayerManager::Update()
{
	// �X�V����
	for (Player* player : players)
	{
		player->Update();
	}

	// �j������
	for (Player* player : removes)
	{
		std::vector<Player*>::iterator it = std::find(players.begin(), players.end(), player);

		if (it != players.end())
		{
			players.erase(it);
		}
		// �e�ۂ̔j������
		delete player;
	}
	// �j�����X�g���N���A
	removes.clear();
}

//�@�`�揈��
void PlayerManager::Render()
{
	for (Player* player : players)
	{
		player->Render();
	}
}

// �v���C���[�o�^
void PlayerManager::Register(Player* player)
{
	players.emplace_back(player);
}


// �v���C���[�폜
void PlayerManager::Remove(Player* player)
{
	// �j�����X�g�ɒǉ�
	removes.insert(player);
}

// �v���C���[�S�폜
void PlayerManager::Clear()
{
	for (Player* player : players)
	{
		delete player;
	}
	players.clear();
}

// �f�o�b�O�pGUI�`��
void PlayerManager::DrawDebugGui()
{
	for (Player* player : players)
	{
		player->DrawDebugGui();
	}
}
