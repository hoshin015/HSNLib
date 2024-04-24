#pragma once
#include "StEnemyData.h"

class DataManager
{
private:
	DataManager() {}
	~DataManager() {}

public:
	static DataManager& Instance()
	{
		static DataManager instance;
		return instance;
	}

	// 敵ロード
	void LoadEnemyData(EnemyData* pData);

	// 敵セーブ
	void SaveEnemyData(EnemyData* pData);

	// スポーンエリアロード
	void LoadSpawnEreaData();
	// スポーンエリアセーブ
	void SaveSpawnEreaData();
};