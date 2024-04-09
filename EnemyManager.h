#pragma once

#include <vector>
#include <set>
#include "Enemy.h"

// �G�l�~�[�}�l�[�W���[
class EnemyManager
{
private:
	EnemyManager() {}
	~EnemyManager() {}

public:
	// �B��̃C���X�^���X�擾
	static EnemyManager& Instance()
	{
		static EnemyManager instance;
		return instance;
	}

	// �X�V����
	void Update();

	//�@�`�揈��
	void Render();

	// �G�l�~�[�o�^
	void Register(Enemy* enemy);

	// �G�l�~�[�폜
	void Remove(Enemy* enemy);

	// �G�l�~�[�S�폜
	void Clear();

	// �G�l�~�[���擾
	int GetEnemyCount() const { return static_cast<int>(enemies.size()); }

	// �G�l�~�[�擾
	Enemy* GetEnemy(int index) { return enemies.at(index); }

	// �f�o�b�O�pGUI�`��
	void DrawDebugGui();

	// �G�l�~�[���v���C���[�ɋ߂����Ƀ\�[�g����
	void SortPlaySortEnemiesByDistanceToPlayerEnemy(DirectX::XMFLOAT3 playerPosition);

	// ���C�L���X�g
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit, int id);

private:
	// �G�l�~�[���m�̏Փ˔���
	void CollisionEnemyVsEnemeis();

private:
	std::vector<Enemy*> enemies;
	std::set<Enemy*> removes;
};