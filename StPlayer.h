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
	//�v���p�e�B&�X�e�[�^�X
	float mobility;
	float rotateSpeed;
	//float speed;
	float accel;
	float slow;
	//DirectX::XMFLOAT2 moveDirection;

	//�p���B
	float parryRadius;
	float parryCooldown;
	float parryCooldownCount;
	float parryKnockback;


	//�q�@

	using InputVariant = std::variant<bool, int, float, DirectX::XMFLOAT2>;
	std::map<std::string, InputVariant> inputMap;

	template<typename T>
	T GetInputMap(std::string str) {
		return std::get<T>(inputMap[str]);
	}

	using debugVariant = std::variant<bool, int, float>;
	std::map<std::string, InputVariant> debugValue;

	template<typename T>
	T GetDebugValue(std::string str) {
		return std::get<T>(debugValue[str]);
	}


};

