#pragma once
#include <memory>
#include "Scene.h"
#include "Library/2D/Sprite.h"
#include "Library/2D/UVScrollSprite.h"
#include "Library/2D/MaskSprite.h"


// �e�X�g�V�[��
class SceneTest : public Scene
{
public:
	SceneTest() {}
	~SceneTest() override {};

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
};
