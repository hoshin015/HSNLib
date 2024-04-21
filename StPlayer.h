#pragma once
#include <variant>
#include <map>

#include "SpinningTopBase.h"
#include "SpinningTopEnemyManager.h"

class StPlayer :public SpinningTopBase
{
public:
	StPlayer();
	~StPlayer() override;

	void Update();
	void Render();
	void DrawDebugGui();

	bool IsCoolDown() { return parryCooldownCount <= 0; }

private:
	void Input();
	void UpdateEDistance();
	void UpdateRotate();
	void UpdateMove();
	void UpdateAttack();
	void UpdateDamage();

	void RenderDebugPrimitive();

protected:
	void OnLanding() override;
	void OnDamaged() override;
	void OnDead() override;

private:
	//パラメータは上に,プロパティは下に書いてる
	std::vector<SpinningTopEnemy*> nearEnemy;

	//移動
	float mobility;
	float accel;
	float slow;

	//回転
	float rotateSpeed;
	float rotateMaxSpeed;

	//パリィ
	bool parry = false;
	float parryDamageRadius = 0;
	float parryCooldownCount = 0;

	bool parryGauge = false;
	float parryGaugeDamageRadius = 0;

	float parryRadius;
	float parryDamageMaxRadius;
	float parryDamageIncrementSpeed;
	float parryCooldown;
	float parryKnockback;

	float parryGaugeRadius;
	float parryGaugeConsumed;
	float parryGaugeDamageMaxRadius;

	//ダメージ
	float damagedKnockback;

	//子機

	//入力
	using InputVariant = std::variant<bool, int, float, DirectX::XMFLOAT2>;
	std::map<std::string, InputVariant> inputMap;

	template<typename T>
	const T& GetInputMap(std::string str) {
		T* result = std::get_if<T>(&inputMap[str]);
		return result ? *result : T();
	}

	//デバッグ用
	using debugVariant = std::variant<bool, int, float ,DirectX::XMFLOAT2>;
	std::map<std::string, debugVariant> debugValue;

	template<typename T>
	T GetDebugValue(std::string str) {
		return std::get<T>(debugValue[str]);
	}


};

