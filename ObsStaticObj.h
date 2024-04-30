#pragma once
#include "Obstacle.h"

class ObsStaticObj : public Obstacle
{
public:
	ObsStaticObj(const char* filePath);
	~ObsStaticObj() override;

	void Update() override;
};