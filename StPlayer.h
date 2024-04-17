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

protected:
	void OnLanding() override;
	void OnDamaged() override;
	void OnDead() override;

private:
	float moveSpeed;
	float rotateSpeed;
	std::map<std::string, std::variant<float, DirectX::XMFLOAT2>> inputAnalogMap;
	std::map<std::string, int> inputDigitalMap;
	//DirectX::XMFLOAT2 vel {};

};

