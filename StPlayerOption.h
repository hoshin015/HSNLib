#pragma once

#include "SpinningTopBase.h"

class StPlayerOption :public SpinningTopBase {
public:
	StPlayerOption();
	~StPlayerOption() override;

	void Update();
	void Render();
	void DrawDebugGui();

private:

};