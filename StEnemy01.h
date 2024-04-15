#include "SpinningTopEnemy.h"


class StEnemy01 : public SpinningTopEnemy
{
public:
	StEnemy01();
	~StEnemy01() override;

	// XVˆ—
	void Update();

	// •`‰æˆ—
	void Render();

protected:
	// €–Sˆ—
	void OnDead() override;

private:
	float radius = 0.5f;
	float height = 1.0f;

};