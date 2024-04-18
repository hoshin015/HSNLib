#pragma once

#pragma once

#include <vector>
#include <set>
#include "Obstacle.h"

// ��Q���}�l�[�W���[
class ObstacleManager
{
private:
	ObstacleManager() {}
	~ObstacleManager() {}

public:
	// �B��̃C���X�^���X�擾
	static ObstacleManager& Instance()
	{
		static ObstacleManager instance;
		return instance;
	}

	// �X�V����
	void Update();

	//�@�`�揈��
	void Render();

	// ��Q���o�^
	void Register(Obstacle* enemy);

	// ��Q���폜
	void Remove(Obstacle* enemy);

	// ��Q���S�폜
	void Clear();

	// ��Q�����擾
	int GetObstacleCount() const { return static_cast<int>(obstacles.size()); }

	// ��Q���擾
	Obstacle* GetObstacle(int index) { return obstacles.at(index); }

	// �f�o�b�O�pGUI�`��
	void DrawDebugGui();

private:
	std::vector<Obstacle*> obstacles;
	std::set<Obstacle*> removes;
};