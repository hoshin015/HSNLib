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

protected:
	void OnLanding() override;
	void OnDamaged() override;
	void OnDead() override;

private:
	float mobility;
	float rotateSpeed;
	float speed;
	float accel;
	DirectX::XMFLOAT2 moveDirection;

	using InputVariant = std::variant<bool, int, float, DirectX::XMFLOAT2>;
	std::map<std::string, InputVariant> inputMap;

	template<typename T>
	T GetInputMap(std::string str) {
		return std::get<T>(inputMap[str]);
	}

#if _DEBUG
	using debugVariant = std::variant<bool, int, float>;
	std::map<std::string, InputVariant> debugValue;

	template<typename T>
	T GetDebugValue(std::string str) {
		return std::get<T>(debugValue[str]);
	}
#endif // _DEBUG


};

