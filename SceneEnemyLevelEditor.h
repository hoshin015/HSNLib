#pragma once
#include <memory>
#include "Scene.h"
#include "Library/3D/SkinnedMesh.h"
#include "Library/3D/SkyMap.h"
#include "StEnemyData.h"

// �G�l�~�[���x���G�f�B�^�[
class SceneEnemyLevelEditor : public Scene
{
public:
	SceneEnemyLevelEditor() {}
	~SceneEnemyLevelEditor() override {};

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
	// �����ʒu�X�V
	void UpdateGeneratePosition();

private:
	std::unique_ptr<SkyMap> skyMap;

	EnemyData subEnemyData;

	DirectX::XMFLOAT3 generatePosition = { 0,0,0 };
};
