#pragma once

#include <vector>
#include <set>
#include "Projectile.h"

// �e�ۃ}�l�[�W���[
class ProjectileManager
{
public:
	ProjectileManager();
	~ProjectileManager();

	// �X�V����
	void Update();

	// �`�揈��
	void Render();

	// �e�ۓo�^
	void Register(Projectile* projectile);

	// �e�ۍ폜
	void Remove(Projectile* projectile);

	// �e�ۑS�폜
	void Clear();

	// �e�ې��擾
	int GetProjectileCount() const { return static_cast<int>(projectiles.size()); }

	// �e�ێ擾
	Projectile* GetProjectile(int index) { return projectiles.at(index); }

private:
	std::vector<Projectile*> projectiles;
	std::set<Projectile*> removes;
};