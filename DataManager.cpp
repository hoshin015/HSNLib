#include "DataManager.h"

#include<iostream>
#include<fstream>
// nlohmann
#include <nlohmann/json.hpp>

#include "StEnemyData.h"



// ロード
void DataManager::LoadData()
{
	std::ifstream enemyFile("Data/Json/enemy.json");
	if (enemyFile.good())
	{
		nlohmann::json j;
		enemyFile >> j;
		int index = 0;
		for (auto& jsonEnemy : j["EnemyData"])
		{
			enemyData[index].enemyType = jsonEnemy["Type"];
			enemyData[index].radius = jsonEnemy["Radius"];
			enemyData[index].pursuitRadius = jsonEnemy["Pursuit"];
			enemyData[index].searchRadius = jsonEnemy["Search"];
			enemyData[index].notSearchRadius = jsonEnemy["NotSearch"];

			index++;
		}
	}
}

// セーブ
void DataManager::SaveData()
{
	using json = nlohmann::json;
	json enemyJson;

	for (int i = 0; i < ENEMY_NUM; i++)
	{
		enemyJson["EnemyData"] +=
		{
			{"Type", enemyData[i].enemyType},
			{ "Radius", enemyData[i].radius },
			{ "Pursuit", enemyData[i].pursuitRadius },
			{ "Search", enemyData[i].searchRadius },
			{ "NotSearch", enemyData[i].notSearchRadius }
		};
	};
	std::ofstream enemyFile("Data/Json/enemy.json");
	enemyFile << std::setw(4) << enemyJson;
	enemyFile.close();
}
