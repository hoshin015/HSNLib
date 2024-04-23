#pragma once
#pragma once

#include <vector>
#include <set>
#include "SpinningTopEnemy.h"
#include "StEnemyData.h"

// �G�l�~�[�}�l�[�W���[
class SpinningTopEnemyManager
{
private:
	SpinningTopEnemyManager() {}
	~SpinningTopEnemyManager() {}

public:
	// �B��̃C���X�^���X�擾
	static SpinningTopEnemyManager& Instance()
	{
		static SpinningTopEnemyManager instance;
		return instance;
	}

	// �X�V����
	void Update();

	//�@�`�揈��
	void Render(bool drawShadow = false);

	// �G�l�~�[�o�^
	void Register(SpinningTopEnemy* enemy);

	// �G�l�~�[�폜
	void Remove(SpinningTopEnemy* enemy);

	// �G�l�~�[�S�폜
	void Clear();

	// �G�l�~�[���擾
	int GetEnemyCount() const { return static_cast<int>(enemies.size()); }

	// �G�l�~�[�擾
	SpinningTopEnemy* GetEnemy(int index) { return enemies.at(index); }

	// �f�o�b�O�pGUI�`��
	void DrawDebugGui();

	// �G�l�~�[���v���C���[�ɋ߂����Ƀ\�[�g����
	void SortPlaySortEnemiesByDistanceToPlayerEnemy(DirectX::XMFLOAT3 playerPosition);

	// ���C�L���X�g
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit, int id);

	// �G�l�~�[�̃X�e�[�^�X�l���X�V
	void UpdateStatusValue(EnemyData* pData);

private:
	// �G�l�~�[���m�̏Փ˔���
	void CollisionEnemyVsEnemeis();

	// �G�l�~�[�Ə�Q���̏Փ˔���
	void CollisionEnemyVsObstacle();

public:
	bool drawDebugPrimitive = true;

private:
	std::vector<SpinningTopEnemy*> enemies;
	std::set<SpinningTopEnemy*> removes;
};