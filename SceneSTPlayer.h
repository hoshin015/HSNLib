#pragma once
#include <memory>
#include "Scene.h"
#include "Library/3D/SkinnedMesh.h"
#include "StageContext.h"

// �e�X�g�V�[��
class SceneSTPlayer : public Scene {
public:
	SceneSTPlayer() {}
	~SceneSTPlayer() override {};

	// ������
	void Initialize() override;

	// �I����
	void Finalize() override;

	// �X�V
	void Update() override;

	// �`��
	void Render() override;

	// �f�o�b�O�`��
	void DrawDebugGUI();

private:
	std::unique_ptr<StageContext> stageMain;
};
