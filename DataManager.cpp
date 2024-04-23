#include "DataManager.h"

#include<iostream>
#include<fstream>
// nlohmann
#include <nlohmann/json.hpp>




// ロード
void DataManager::LoadEnemyData(EnemyData* pData)
{
	// enemyData のロード
	std::ifstream enemyFile("Data/Json/enemy.json");
	if (enemyFile.good())
	{
		nlohmann::json j;
		enemyFile >> j;
		int index = 0;
		for (auto& jsonEnemy : j["EnemyData"])
		{
			pData[index].enemyKind = jsonEnemy["Kind"];
			pData[index].behaviorType = jsonEnemy["BehaviorType"];
			pData[index].radius = jsonEnemy["Radius"];
			pData[index].pursuitRadius = jsonEnemy["Pursuit"];
			pData[index].searchRadius = jsonEnemy["Search"];
			pData[index].notSearchRadius = jsonEnemy["NotSearch"];

			index++;
		}
	}
}

// セーブ
void DataManager::SaveEnemyData(EnemyData* pData)
{
	// enemyData のセーブ
	using json = nlohmann::json;
	json enemyJson;

	for (int i = 0; i < ENEMY_NUM; i++)
	{
		enemyJson["EnemyData"] +=
		{
			{ "Kind", pData[i].enemyKind },
			{ "BehaviorType", pData[i].behaviorType },
			{ "Radius", pData[i].radius },
			{ "Pursuit", pData[i].pursuitRadius },
			{ "Search", pData[i].searchRadius },
			{ "NotSearch", pData[i].notSearchRadius }
		};
	};
	std::ofstream enemyFile("Data/Json/enemy.json");
	enemyFile << std::setw(4) << enemyJson;
	enemyFile.close();
}