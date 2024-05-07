#pragma once
#include "SpinningTopBase.h"
#include "StPlayerData.h"

class StMenuPlayer :public SpinningTopBase {
public:
	StMenuPlayer();
	~StMenuPlayer() {}

	void Update();
	void Render();

private:
	std::unique_ptr<PlayerData> data;
};