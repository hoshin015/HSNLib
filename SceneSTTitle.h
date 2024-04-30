#pragma once
#include <memory>
#include "Scene.h"
#include "Library/2D/Sprite.h"
#include "Library/2D/UVScrollSprite.h"
#include "Library/2D/MaskSprite.h"


// �^�C�g���V�[��
class SceneSTTitle : public Scene {
public:
	SceneSTTitle() {}
	~SceneSTTitle() override {};

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
	DirectX::XMFLOAT2 _videoPos = {0,0};
	DirectX::XMFLOAT2 _videoSize = { 1280,720 };
	DirectX::XMFLOAT4 _videoColor = { 1,1,1,1 };
};
