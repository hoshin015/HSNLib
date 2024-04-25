#pragma once
#include "StEnemyData.h"
#include "StPlayerData.h"

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
	void LoadPlayerData(PlayerData* pData ,size_t arraySize);

	// �Z�[�u
	void SaveEnemyData(EnemyData* pData);
	void SavePlayerData(PlayerData* pData ,size_t arraySize);
};