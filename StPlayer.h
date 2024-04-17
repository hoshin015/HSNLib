#pragma once
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
	std::vector<float> a;
	DirectX::XMFLOAT2 vel {};

};

