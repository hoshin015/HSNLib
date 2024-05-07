#pragma once
#include <memory>
#include <vector>
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
	std::vector<std::unique_ptr<Sprite>> sprite;
	std::unique_ptr<MaskSprite> maskSprite;
	Primitive2D fadeout;
	float fadeoutA = 0;
	DirectX::XMFLOAT4 _videoColor = { 1,1,1,1 };
	float rotate = 0;

	DirectX::XMFLOAT2 dPos = {};
	DirectX::XMFLOAT2 dPos2 = {};
	DirectX::XMFLOAT2 dSize = {};
	DirectX::XMFLOAT2 dSize2 = {};
};
