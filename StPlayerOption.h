#pragma once

#include "StPlayerBase.h"

class StPlayerOption :public StPlayerBase {
public:
	StPlayerOption(std::shared_ptr<SkinnedMesh> model);
	~StPlayerOption() override;

	void Update() override;
	void Render() override;
	void DrawDebugGui() override;

private:

private:
	StPlayerBase* parent;

};