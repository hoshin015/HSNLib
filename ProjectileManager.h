#pragma once

#include <vector>
#include <set>
#include "Projectile.h"

// ’eŠÛƒ}ƒl[ƒWƒƒ[
class ProjectileManager
{
public:
	ProjectileManager();
	~ProjectileManager();

	// XVˆ—
	void Update();

	// •`‰æˆ—
	void Render();

	// ’eŠÛ“o˜^
	void Register(Projectile* projectile);

	// ’eŠÛíœ
	void Remove(Projectile* projectile);

	// ’eŠÛ‘Síœ
	void Clear();

	// ’eŠÛ”æ“¾
	int GetProjectileCount() const { return static_cast<int>(projectiles.size()); }

	// ’eŠÛæ“¾
	Projectile* GetProjectile(int index) { return projectiles.at(index); }

private:
	std::vector<Projectile*> projectiles;
	std::set<Projectile*> removes;
};