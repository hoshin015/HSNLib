#pragma once
#include <variant>
#include <map>
#include <vector>

#include "SpinningTopBase.h"
#include "SpinningTopEnemyManager.h"
#include "StPlayerData.h"
#include "ParryEffect.h"
#include "DomeEffect.h"

class StPlayerBase :public SpinningTopBase {
public:
	virtual void Update() = 0;
	virtual void Render() = 0;
	virtual void DrawDebugGui() {}
	static void UpdateEDistance(const std::vector<StPlayerBase*>& players);

	bool IsCoolDown() { return parryCooldownCount <= 0; }
	bool IsPlayer() { return isPlayer; }
	void Input();

	bool GetParryGauge() { return parryGauge; }
	bool GetParry() { return parry; }
	PlayerData* GetData() { return data.get(); }
	float GetRotationSpeed() { return rotateSpeed; }
	float parryDamageRadius = 0;
	float parryGaugeDamageRadius = 0;

	int partsCount = 0;

	// 子機
	virtual void AddOption() {};
	virtual void EraseOption() {};

	static void SetRotateSpeed(float speed) { rotateSpeed = speed; }
private:
	//void UpdateRotate();
	//void UpdateMove();
	//void UpdateDamaged();
	//void RenderDebugPrimitive();

public:
	bool isDead = false;
	float totalHitScore = 0;
	

protected:
	void UpdateAttack();
	void RenderDebugPrimitive();

	//パラメータは上に,プロパティは下に書いてる
	inline static std::vector<SpinningTopEnemy*> nearEnemy;

	std::shared_ptr<PlayerData> data;
	std::shared_ptr<PlayerData> optionData;
	bool isPlayer;

	//移動

	//回転
	inline static float rotateSpeed;

	//パリィ
	std::unique_ptr<ParryEffect> parryEffect;
	std::unique_ptr<DomeEffect> domeEffect;
	inline static bool parry = false;


	inline static float parryCooldownCount = 0;

	inline static bool parryGauge = false;

	//ダメージ


	//入力
public:
	using InputVariant = std::variant<bool, int, float, DirectX::XMFLOAT2>;
	inline static std::map<std::string, InputVariant> inputMap;

	template<typename T>
	static const T GetInputMap(std::string str) {
		return std::get<T>(inputMap[str]);
	}

protected:
	//デバッグ用
	using debugVariant = std::variant<bool, int, float, DirectX::XMFLOAT2>;
	std::map<std::string, debugVariant> debugValue;

	template<typename T>
	T GetDebugValue(std::string str) {
		return std::get<T>(debugValue[str]);
	}


};