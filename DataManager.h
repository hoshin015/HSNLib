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

	// ロード
	void LoadEnemyData(EnemyData* pData);

	// セーブ
	void SaveEnemyData(EnemyData* pData);
};