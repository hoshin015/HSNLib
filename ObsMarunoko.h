#pragma once
#include "Obstacle.h"

class ObsMarunoko : public Obstacle
{
public:
	ObsMarunoko(const char* filePath);
	~ObsMarunoko() override;

	void Update() override;

private:
	float rotationSpeed = 360.0f;	// 1•b‚É‰ñ“]‚·‚éŠp“x
};