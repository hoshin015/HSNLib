#pragma once
#include <variant>
#include <map>

#include "SpinningTopBase.h"
#include "Collision.h"

class StPlayer :public SpinningTopBase
{
public:
	StPlayer();
	~StPlayer() override;

	void Update();
	void Render();
	void DrawDebugGui();

private:
	void Input();
	void UpdateMove();
	void UpdateAttack();

protected:
	void OnLanding() override;
	void OnDamaged() override;
	void OnDead() override;

private:
	//パラメータは上に,プロパティは下に書いてる
	//移動
	float mobility;
	float accel;
	float slow;

	//回転
	float rotateIncrementSpeed;
	float rotateIncrement;

	//パリィ
	bool parry = false;
	float parryDamageRadius = 0;
	float parryCooldownCount = 0;

	float parryRadius;
	float parryDamageMaxRadius;
	float parryDamageIncrementSpeed;
	float parryCooldown;
	float parryKnockback;

	//子機

	//入力
	using InputVariant = std::variant<bool, int, float, DirectX::XMFLOAT2>;
	std::map<std::string, InputVariant> inputMap;

	template<typename T>
	T GetInputMap(std::string str) {
		return std::get<T>(inputMap[str]);
	}

	//デバッグ用
	using debugVariant = std::variant<bool, int, float ,DirectX::XMFLOAT2>;
	std::map<std::string, debugVariant> debugValue;

	template<typename T>
	T GetDebugValue(std::string str) {
		return std::get<T>(debugValue[str]);
	}


};

