#pragma once
#include "Obstacle.h"
#include "EnemySpawnEffect.h"

class ObsMarunoko : public Obstacle
{
public:
	ObsMarunoko(const char* filePath, int type, float speed);
	~ObsMarunoko() override;

	void Update() override;

	void Render();

	enum MarunokoType
	{
		NO_MOVE,
		VERTICAL,
		HORIZONTAL,
	};

	MarunokoType type;


	DirectX::XMFLOAT3 spawnPos = { 0,0,0 };
	std::unique_ptr<EnemySpawnEffect> spawnEffect;
private:
	float rotationSpeed = 360.0f;	// 1•b‚É‰ñ“]‚·‚éŠp“x
	float moveSpeed = 3.0f;

	
	bool isSpawnFinish = false;
	float spawnWaitTime = 3.0f;
	float spawnWiatTimer = 0.0f;
};