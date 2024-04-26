#pragma once
#include <memory>
#include "Scene.h"
#include "Library/3D/SkinnedMesh.h"
#include "Library/3D/SkyMap.h"
#include "StEnemyData.h"

// �X�|�[���G�f�B�^�[
class SceneSpawnEditor : public Scene
{
public:
	SceneSpawnEditor() {}
	~SceneSpawnEditor() override {};

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

	DirectX::XMFLOAT3 targetPosition = { 0,0,0 };

	int enemyNum = 1;
};
