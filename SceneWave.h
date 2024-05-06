#pragma once
#include <memory>
#include "Scene.h"
#include "Library/3D/SkinnedMesh.h"
#include "Library/3D/SkyMap.h"
#include "Library/2D/Sprite.h"
#include "Pause.h"

// �e�X�g�V�[��
class SceneWave : public Scene
{
public:
	SceneWave() {}
	~SceneWave() override {};

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
	std::unique_ptr<SkyMap> skyMap;

	float gaussianPower = 16.0f;

	// --- �|�[�Y��� ---
	bool isPause = false;
	std::unique_ptr<Pause> pause;
};
