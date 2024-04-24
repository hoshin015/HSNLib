#include "DataManager.h"

#include<iostream>
#include<fstream>
// nlohmann
#include <nlohmann/json.hpp>


// 敵ロード
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
			pData[index].maxMoveSpeed = jsonEnemy["MaxMoveSpeed"];
			pData[index].maxHealth = jsonEnemy["MaxHealth"];

			pData[index].pursuitRadius = jsonEnemy["Pursuit"];
			pData[index].searchRadius = jsonEnemy["Search"];
			pData[index].notSearchRadius = jsonEnemy["NotSearch"];

			pData[index].circleDistance = jsonEnemy["CircleDistance"];
			pData[index].circleRadius = jsonEnemy["CircleRadius"];
			pData[index].wanderAngleChange = jsonEnemy["WanderAngleChange"];
			pData[index].wanderAngleChangeTime = jsonEnemy["WanderAngleChangeTime"];

			pData[index].chargeAttackCoolTime = jsonEnemy["ChargeAttakCoolTime"];
			pData[index].waitChargeAttackTime = jsonEnemy["WaitChargeAttakTime"];

			index++;
		}
	}
}

// 敵セーブ
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
			{ "MaxMoveSpeed", pData[i].maxMoveSpeed },
			{ "MaxHealth", pData[i].maxHealth },

			{ "Pursuit", pData[i].pursuitRadius },
			{ "Search", pData[i].searchRadius },
			{ "NotSearch", pData[i].notSearchRadius },

			{ "CircleDistance", pData[i].circleDistance },
			{ "CircleRadius", pData[i].circleRadius },
			{ "WanderAngleChange", pData[i].wanderAngleChange },
			{ "WanderAngleChangeTime", pData[i].wanderAngleChangeTime },

			{ "ChargeAttakCoolTime", pData[i].chargeAttackCoolTime },
			{ "WaitChargeAttakTime", pData[i].waitChargeAttackTime },
		};
	};
	std::ofstream enemyFile("Data/Json/enemy.json");
	enemyFile << std::setw(4) << enemyJson;
	enemyFile.close();
}

// スポーンエリアロード
void DataManager::LoadSpawnEreaData()
{
	// spawnEreaData のロード
	std::ifstream spawnEreaFile("Data/Json/spawnErea.json");
	if (spawnEreaFile.good())
	{
		nlohmann::json j;
		spawnEreaFile >> j;
		int index = 0;
		for (auto& jsonSpawnErea : j["SpawnEreaData"])
		{
			enemySpawnErea[index].position.x = jsonSpawnErea["PositionX"];
			enemySpawnErea[index].position.y = jsonSpawnErea["PositionY"];
			enemySpawnErea[index].position.z = jsonSpawnErea["PositionZ"];
			enemySpawnErea[index].radius = jsonSpawnErea["Radius"];

			index++;
		}
	}
}

// スポーンエリアセーブ
void DataManager::SaveSpawnEreaData()
{
	// spawnEreaData のセーブ
	using json = nlohmann::json;
	json spawnJson;

	for (int i = 0; i < EREA_NUM; i++)
	{
		spawnJson["SpawnEreaData"] +=
		{
			{ "PositionX", enemySpawnErea[i].position.x },
			{ "PositionY", enemySpawnErea[i].position.y },
			{ "PositionZ", enemySpawnErea[i].position.z },
			{ "Radius", enemySpawnErea[i].radius },
		};
	};
	std::ofstream spawnEreaFile("Data/Json/spawnErea.json");
	spawnEreaFile << std::setw(4) << spawnJson;
	spawnEreaFile.close();
}
