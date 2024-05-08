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

	// �X�R�A�v�Z
	void CalcScore();

	// --- ���U���g��� ---
	bool isResult = false;
	bool resultFinish = false;
	float resultFinishTime = 6.0f;
	float waitTime = 2.0f;		// �v���C���[���S����̑҂�����
	float waitTimer = 0.0f;		// �v���C���[���S���炷����UI�\�������Ȃ����߂Ɏg��
	std::unique_ptr<UiSprite> sprMainMenu;
	std::unique_ptr<UiSprite> sprSideBar1;
	std::unique_ptr<UiSprite> sprSideBar2;

	std::unique_ptr<UiSprite> sprRankS;

	// �e�L�X�g
	std::unique_ptr<UiSprite> sprTotalScore;
	std::unique_ptr<UiSprite> sprAliveTime;

	std::unique_ptr<UiSprite> sprTotalDestoryEnemy;
	std::unique_ptr<UiSprite> sprGetTotalOption;
	std::unique_ptr<UiSprite> sprNowWave;
	std::unique_ptr<UiSprite> sprTotalHitDamage;


	// ���ꂼ��̕\���̊J�n����
	float startRank = 5.0f;

	float startTotalScore = 4.0f;
	float startAliveTime = 4.0f;

	float startTotalDestoryEnemy = 2.0f;
	float startTotalGetOption = 2.3f;
	float startNowWave = 2.6f;
	float startTotalHitDamage = 2.9f;
};