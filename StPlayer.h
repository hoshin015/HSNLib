#pragma once
#include <variant>
#include <map>

#include "StPlayerBase.h"
#include "SpinningTopEnemyManager.h"
#include "StPlayerOption.h"
#include "StPlayerData.h"

class StPlayer :public StPlayerBase
{
public:
	StPlayer();
	~StPlayer() override;

	void Update() override;
	void Render() override;
	void DrawDebugGui() override;

private:
	void UpdateRotate();
	void UpdateMove();
	void UpdateDamaged();

	void UpdateObstacleCollision();

	void UpdateOption();
	void AddOption();
	void EraseOption();
protected:
	void OnLanding() override;
	void OnDamaged() override;
	void OnDead() override;

private:
	bool isDead = false;

	//パラメータは上に,プロパティは下に書いてる
	//std::vector<SpinningTopEnemy*> nearEnemy;

	//PlayerData data;

	//移動
	//float mobility;
	//float accel;
	//float slow;

	//回転
	//float rotateSpeed;
	//float rotateMaxSpeed;

	//パリィ
	//bool parry = false;
	//float parryDamageRadius = 0;
	//float parryCooldownCount = 0;

	//bool parryGauge = false;
	//float parryGaugeDamageRadius = 0;

	//float parryRadius;
	//float parryDamageMaxRadius;
	//float parryDamageIncrementSpeed;
	//float parryCooldown;
	//float parryKnockback;

	//float parryGaugeRadius;
	//float parryGaugeConsumed;
	//float parryGaugeDamageMaxRadius;

	//ダメージ
	//float damagedKnockback;

	//子機
	std::shared_ptr<SkinnedMesh> childModel;
	std::vector<StPlayerOption*> option;

	float optionAngle = 0;

	float invicibleTime = 0.2f;


	//入力
	//using InputVariant = std::variant<bool, int, float, DirectX::XMFLOAT2>;
	//std::map<std::string, InputVariant> inputMap;

	//template<typename T>
	//const T& GetInputMap(std::string str) {
	//	T* result = std::get_if<T>(&inputMap[str]);
	//	return result ? *result : T();
	//}

	//デバッグ用
	//using debugVariant = std::variant<bool, int, float ,DirectX::XMFLOAT2>;
	//std::map<std::string, debugVariant> debugValue;

	//template<typename T>
	//T GetDebugValue(std::string str) {
	//	return std::get<T>(debugValue[str]);
	//}

	template<typename T>
	T GetDebugValue(std::string str) {
		return std::get<T>(debugValue[str]);
	}


public:
	int partsCount = 0;
};

