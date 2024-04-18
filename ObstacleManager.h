#pragma once

#pragma once

#include <vector>
#include <set>
#include "Obstacle.h"

// áQ¨}l[W[
class ObstacleManager
{
private:
	ObstacleManager() {}
	~ObstacleManager() {}

public:
	// BêÌCX^Xæ¾
	static ObstacleManager& Instance()
	{
		static ObstacleManager instance;
		return instance;
	}

	// XV
	void Update();

	//@`æ
	void Render();

	// áQ¨o^
	void Register(Obstacle* enemy);

	// áQ¨í
	void Remove(Obstacle* enemy);

	// áQ¨Sí
	void Clear();

	// áQ¨æ¾
	int GetObstacleCount() const { return static_cast<int>(obstacles.size()); }

	// áQ¨æ¾
	Obstacle* GetObstacle(int index) { return obstacles.at(index); }

	// fobOpGUI`æ
	void DrawDebugGui();

private:
	std::vector<Obstacle*> obstacles;
	std::set<Obstacle*> removes;
};