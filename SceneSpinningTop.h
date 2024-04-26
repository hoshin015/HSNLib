#pragma once
#include <memory>
#include "Scene.h"
#include "Library/3D/SkinnedMesh.h"
#include "Library/3D/SkyMap.h"

// �e�X�g�V�[��
class SceneSpinningTop : public Scene
{
public:
	SceneSpinningTop() {}
	~SceneSpinningTop() override {};

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
};
