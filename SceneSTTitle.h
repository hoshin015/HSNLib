#pragma once
#include <memory>
#include "Scene.h"
#include "Library/2D/Sprite.h"
#include "Library/2D/MaskSprite.h"
#include "Library/2D/Primitive2D.h"

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
	std::unique_ptr<Sprite> titleSprite;
	std::unique_ptr<MaskSprite> maskSprite;
	Primitive2D fadeout;
	float fadeoutA = 0;
	DirectX::XMFLOAT4 _videoColor = { 1,1,1,1 };
};
