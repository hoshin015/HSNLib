#pragma once
#include "Library/3D/SkinnedMesh.h"
#include "Library/3D/SkyMap.h"
#include "Library/2D/Primitive2D.h"

#include "Scene.h"
#include "StageContext.h"
#include "StMenuPlayer.h"
#include "VideoUI.h"

#include <memory>


class SceneSTTutorial : public Scene {
public:
	SceneSTTutorial() {}
	~SceneSTTutorial() override {};

	// ������
	void Initialize() override;

	// �I����
	void Finalize() override;

	// �X�V
	void Update() override;
	void UpdateState();

	// �`��
	void Render() override;

	// �f�o�b�O�`��
	void DrawDebugGUI();

private:
	std::unique_ptr<SkyMap> skyMap;
	std::unique_ptr<StageContext> stageMain;
	std::vector<std::shared_ptr<Sprite>> sprite;
	VideoUI videoUI;
	float gaussianPower = 16;

	DirectX::XMFLOAT2 pos = { 640,360 };
	DirectX::XMFLOAT2 size = { 640,360 };


};
