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

	// �G���[�h
	void LoadEnemyData(EnemyData* pData);

	// �G�Z�[�u
	void SaveEnemyData(EnemyData* pData);

	// �X�|�[���G���A���[�h
	void LoadSpawnEreaData();
	// �X�|�[���G���A�Z�[�u
	void SaveSpawnEreaData();
};