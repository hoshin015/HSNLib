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

protected:
	void OnLanding() override;
	void OnDamaged() override;
	void OnDead() override;
private:

};

