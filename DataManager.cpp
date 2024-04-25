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

void DataManager::LoadPlayerData(PlayerData* pData)
{
	// enemyData のロード
	std::ifstream playerFile("Data/Json/player.json");
	if (playerFile.good()) {
		nlohmann::json j;
		playerFile >> j;

		pData->radius = j["Radius"];

		pData->mobility = j["Mobility"];
		pData->accel = j["Accel"];
		pData->slow = j["Slow"];

		pData->rotateInitialSpeed = j["RotateInitialSpeed"];
		pData->rotateMaxSpeed = j["RotateMaxSpeed"];

		pData->parryRadius = j["ParryRadius"];
		pData->parryDamageMaxRadius = j["ParryDamageMaxRadius"];
		pData->parryDamageIncrementSpeed = j["ParryDamageIncrementSpeed"];
		pData->parryCooldown = j["ParryCooldown"];
		pData->parryKnockback = j["ParryKnockback"];
		pData->parryGaugeRadius = j["ParryGaugeRadius"];
		pData->parryGaugeConsumed = j["ParryGaugeConsumed"];
		pData->parryGaugeDamageMaxRadius = j["ParryGaugeDamageMaxRadius"];

		pData->damagedKnockback = j["DamagedKnockback"];

		pData->optionRange = j["OptionRange"];
	}
	else {
		*pData = PlayerData();
	}
	playerFile.close();
}

void DataManager::SavePlayerData(PlayerData* pData)
{
	using json = nlohmann::json;
	json playerJson;

	playerJson += {
		{"Radius", pData->radius },
		{"Mobility",pData->mobility},
		{"Accel",pData->accel},
		{"Slow",pData->slow},
		{"RotateInitialSpeed",pData->rotateInitialSpeed },
		{"RotateMaxSpeed",pData->rotateMaxSpeed },
		{"ParryRadius",pData->parryRadius },
		{"ParryDamageMaxRadius",pData->parryDamageMaxRadius },
		{"ParryDamageIncrementSpeed",pData->parryDamageIncrementSpeed },
		{"ParryCooldown",pData->parryCooldown },
		{"ParryKnockback",pData->parryKnockback },
		{"ParryGaugeRadius",pData->parryGaugeRadius },
		{"ParryGaugeConsumed",pData->parryGaugeConsumed },
		{"ParryGaugeDamageMaxRadius",pData->parryGaugeDamageMaxRadius },
		{"DamagedKnockback",pData->damagedKnockback },
		{"OptionRange",pData->optionRange },
	};

	std::ofstream playerFile("Data/Json/player.json");
	playerFile << std::setw(4) << playerJson;
	playerFile.close();
}

