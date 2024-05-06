#pragma once
#include "UiSprite.h"

class Result
{
public:
	Result();
	~Result() {};

	// �X�V
	void Update();

	// �`��
	void Render();

	// --- ���U���g��� ---
	bool isResult = false;
	float waitTime = 2.0f;		// �v���C���[���S����̑҂�����
	float waitTimer = 0.0f;		// �v���C���[���S���炷����UI�\�������Ȃ����߂Ɏg��
	std::unique_ptr<UiSprite> sprMainMenu;
	std::unique_ptr<UiSprite> sprSideBar1;
	std::unique_ptr<UiSprite> sprSideBar2;
};