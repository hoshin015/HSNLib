#pragma once
#include "Library/3D/Sprite3D.h"

#include "Scene.h"
#include "StageContext.h"

#include <memory>

class Sprite3DObject :public Sprite3D {
public:
	Sprite3DObject(const wchar_t* filePath) :Sprite3D(filePath) {}

private:

};

class SceneSTMainMenu : public Scene {
public:
	SceneSTMainMenu() {}
	~SceneSTMainMenu() override {};

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新
	void Update() override;

	// 描画
	void Render() override;

	// デバッグ描画
	void DrawDebugGUI();

private:
	const DirectX::XMFLOAT2 tutorialS3DAspectRatio = { 1,512/256 };
	std::unique_ptr<Sprite3D> tutorialS3D;
	DirectX::XMFLOAT4X4 tS3DTransform = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
	DirectX::XMFLOAT3 tS3DPosition = { 0,0.05f,1 };
	DirectX::XMFLOAT3 tS3DAngle = { DirectX::XM_PIDIV2,DirectX::XM_PI,0 };
	float tS3DScale = 1;

	std::unique_ptr<StageContext> stageMain;

};
