#pragma once
#include "Obstacle.h"

class ObsParts : public Obstacle
{
public:
	ObsParts(const char* filePath);
	~ObsParts() override;

	void Update() override;

private:
	DirectX::XMFLOAT3 velocity = { 0,0,0 };
	float frictionPower = 5.0f;

	float rotationSpeed = 0;	// 1•b‚É‰ñ“]‚·‚éŠp“x

	int maxVelocity = 20.0f;
	int maxFrictionPower = 5.0f;
	int minFrictionPower = 3.0f;
	int maxRotationSpeed = 120.0f;
	int minRotationSpeed = 30.0f;
};