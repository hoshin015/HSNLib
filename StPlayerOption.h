#pragma once

#include "StPlayerBase.h"
#include "StPlayerData.h"

class StPlayerOption :public StPlayerBase {
public:
	StPlayerOption(std::shared_ptr<SkinnedMesh> model, std::shared_ptr<PlayerData>& data);
	~StPlayerOption() override;

	void Update() override;
	void Render() override;
	void DrawDebugGui() override;

private:

private:
	StPlayerBase* parent;

};