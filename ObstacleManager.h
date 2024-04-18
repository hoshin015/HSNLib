#pragma once

#pragma once

#include <vector>
#include <set>
#include "Obstacle.h"

// 障害物マネージャー
class ObstacleManager
{
private:
	ObstacleManager() {}
	~ObstacleManager() {}

public:
	// 唯一のインスタンス取得
	static ObstacleManager& Instance()
	{
		static ObstacleManager instance;
		return instance;
	}

	// 更新処理
	void Update();

	//　描画処理
	void Render();

	// 障害物登録
	void Register(Obstacle* enemy);

	// 障害物削除
	void Remove(Obstacle* enemy);

	// 障害物全削除
	void Clear();

	// 障害物数取得
	int GetObstacleCount() const { return static_cast<int>(obstacles.size()); }

	// 障害物取得
	Obstacle* GetObstacle(int index) { return obstacles.at(index); }

	// デバッグ用GUI描画
	void DrawDebugGui();

private:
	std::vector<Obstacle*> obstacles;
	std::set<Obstacle*> removes;
};