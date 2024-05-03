#pragma once
#include "Obstacle.h"

class ObsMarunoko : public Obstacle
{
public:
	ObsMarunoko(const char* filePath, int type, float speed);
	~ObsMarunoko() override;

	void Update() override;

	enum MarunokoType
	{
		NO_MOVE,
		VERTICAL,
		HORIZONTAL,
	};

	MarunokoType type;
private:
	float rotationSpeed = 360.0f;	// 1•b‚É‰ñ“]‚·‚éŠp“x
	float moveSpeed = 3.0f;
};