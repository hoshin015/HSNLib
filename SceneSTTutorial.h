#pragma once
#include "Library/3D/SkinnedMesh.h"
#include "Library/3D/SkyMap.h"
#include "Library/2D/Primitive2D.h"
#include "Library/Text/DispString.h"

#include "Scene.h"
#include "StageContext.h"
#include "StMenuPlayer.h"
#include "VideoUI.h"
#include "Pause.h"

#include <memory>
#include <map>

class SceneSTTutorial : public Scene {
public:
	SceneSTTutorial() {}
	~SceneSTTutorial() override {};

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新
	void Update() override;
	void UpdateState();
	void UpdateTutorial();

	// 描画
	void Render() override;

	// デバッグ描画
	void DrawDebugGUI();

private:
	enum TUTORIALSTATE {
		PARRY = 0,
		ATTACK,
		GAUGEATTACK,
		GETOPTION,
		SPSTART,
		START,
		CLEAR,
		BACKTOTUTORIAL,
		PARRY2,
		PARRYALWAYS,
		KEYBORD,
		CONTROLLER,
		COUNT
	};
	int tState = PARRY;

	std::unique_ptr<SkyMap> skyMap;
	std::unique_ptr<StageContext> stageMain;
	std::unique_ptr<Pause> pause;
	float gaussianPower = 16;

	std::vector<std::unique_ptr<Sprite>> sprite;
	std::vector<std::unique_ptr<Sprite>> spriteControl;
	std::vector<std::unique_ptr<Sprite>> spriteUI;
	DirectX::XMFLOAT2 sPos = {};
	DirectX::XMFLOAT2 dPos = {};
	DirectX::XMFLOAT2 sSize = {};
	DirectX::XMFLOAT2 dSize = {};

	float time = 0;
	float spawnTime = 0;
	float ease = 0;
	int Efunc = 0;
	int tarm = -1;
	int text = 0;
	int nextPage = 0;

	VideoUI videoUI;

	std::map<std::string, bool> stateMap;
	Primitive2D rect;
};
