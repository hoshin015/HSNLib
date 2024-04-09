#include "ProjectileManager.h"

// �R���X�g���N�^
ProjectileManager::ProjectileManager()
{
}

// �f�X�g���N�^
ProjectileManager::~ProjectileManager()
{
	Clear();
}

// �X�V����
void ProjectileManager::Update()
{
	// �X�V����
	for (Projectile* projectile : projectiles)
	{
		projectile->Update();
	}

	// �j������
	for (Projectile* projectile : removes)
	{
		std::vector<Projectile*>::iterator it = std::find(projectiles.begin(), projectiles.end(), projectile);

		if (it != projectiles.end())
		{
			projectiles.erase(it);
		}
		// �e�ۂ̔j������
		delete projectile;
	}
	// �j�����X�g���N���A
	removes.clear();
}

// �`�揈��
void ProjectileManager::Render()
{
	for (Projectile* projectile : projectiles)
	{
		projectile->Render();
	}
}

// �e�ۓo�^
void ProjectileManager::Register(Projectile* projectile)
{
	projectiles.push_back(projectile);
}

// �e�ۍ폜
void ProjectileManager::Remove(Projectile* projectile)
{
	// �j�����X�g�ɒǉ�
	removes.insert(projectile);
}

// �e�ۑS�폜
void ProjectileManager::Clear()
{
	for (Projectile* projectile : projectiles)
	{
		delete projectile;
	}
	projectiles.clear();
}

