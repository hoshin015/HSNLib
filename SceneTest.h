#pragma once
#include <memory>
#include "Scene.h"
#include "Library/2D/Sprite.h"


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
	std::unique_ptr<Sprite> sprite;

	float timer = 0;
	float time = 2.0f;

	float nowPos = 0;
	float startPos = 0;
	float endPos = 640;
};
