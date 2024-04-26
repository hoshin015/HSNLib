#pragma once
#include <variant>
#include <map>
#include <vector>

#include "SpinningTopBase.h"
#include "SpinningTopEnemyManager.h"
#include "StPlayerData.h"

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
	float parryDamageRadius = 0;
	float parryGaugeDamageRadius = 0;


private:
	//void UpdateRotate();
	//void UpdateMove();
	//void UpdateDamaged();
	//void RenderDebugPrimitive();

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
	inline static bool parry = false;


	inline static float parryCooldownCount = 0;

	inline static bool parryGauge = false;

	//ダメージ



	//入力
	using InputVariant = std::variant<bool, int, float, DirectX::XMFLOAT2>;
	inline static std::map<std::string, InputVariant> inputMap;

	template<typename T>
	const T& GetInputMap(std::string str) {
		T* result = std::get_if<T>(&inputMap[str]);
		return result ? *result : T();
	}

	//デバッグ用
	using debugVariant = std::variant<bool, int, float, DirectX::XMFLOAT2>;
	std::map<std::string, debugVariant> debugValue;

	template<typename T>
	T GetDebugValue(std::string str) {
		return std::get<T>(debugValue[str]);
	}


};