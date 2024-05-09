#pragma once
#include <memory>
#include "Scene.h"
#include "Library/3D/SkinnedMesh.h"
#include "Library/3D/SkyMap.h"
#include "Library/2D/Sprite.h"
#include "Pause.h"
#include "Result.h"

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
	//std::unique_ptr<SkyMap> skyMap;

	float gaussianPower = 16.0f;

	// --- �|�[�Y��� ---
	std::unique_ptr<Pause> pause;

	// --- ���U���g��� ---
	std::unique_ptr<Result> result;

	// �Q�[��UI
	std::unique_ptr<Sprite> life3;
	std::unique_ptr<Sprite> life2;
	std::unique_ptr<Sprite> life1;
	std::unique_ptr<Sprite> life0;

	std::unique_ptr<Sprite> sprRotSpeedTop;
	std::unique_ptr<Sprite> sprRotSpeedMiddle;
	std::unique_ptr<Sprite> sprRotSpeedMiddleMask;
	std::unique_ptr<Sprite> sprRotSpeedBottom;
	
	std::unique_ptr<Sprite> sprWaveBar;
	std::unique_ptr<Sprite> sprWaveBarBg;
	std::unique_ptr<Sprite> sprWave1;
	std::unique_ptr<Sprite> sprWave2;
	std::unique_ptr<Sprite> sprWave3;
	std::unique_ptr<Sprite> sprWave4;

	// ���O�Ƀ��f����ǂݍ���ł���
	std::shared_ptr<SkinnedMesh> plOptionModel;
	std::shared_ptr<SkinnedMesh> enemySpawnModel;
	std::shared_ptr<SkinnedMesh> enemyModel1;
	std::shared_ptr<SkinnedMesh> enemyModel1Top;
	std::shared_ptr<SkinnedMesh> enemyModel1Middle;
	std::shared_ptr<SkinnedMesh> enemyModel1Bottom;
	std::shared_ptr<SkinnedMesh> enemyModel2;
	std::shared_ptr<SkinnedMesh> enemyModel2Top;
	std::shared_ptr<SkinnedMesh> enemyModel2Middle;
	std::shared_ptr<SkinnedMesh> enemyModel2Bottom;
	std::shared_ptr<SkinnedMesh> obsMarunoko;
};
