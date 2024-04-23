#pragma once

#include "StPlayer.h"

class StPlayerOption :public SpinningTopBase {
public:
	StPlayerOption(std::shared_ptr<SkinnedMesh> model);
	~StPlayerOption() override;

	void Update();
	void Render();
	void DrawDebugGui();

private:

};