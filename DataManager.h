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

	// ���[�h
	void LoadEnemyData(EnemyData* pData);

	// �Z�[�u
	void SaveEnemyData(EnemyData* pData);
};