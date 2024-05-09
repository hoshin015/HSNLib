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
	std::unique_ptr<UiSprite> sprRankA;
	std::unique_ptr<UiSprite> sprRankB;
	std::unique_ptr<UiSprite> sprRankC;

	std::unique_ptr<UiSprite> sprHighScore;

	// �e�L�X�g
	std::unique_ptr<UiSprite> sprTotalScore;
	std::unique_ptr<UiSprite> sprAliveTime;

	std::unique_ptr<UiSprite> sprTotalDestoryEnemy;
	std::unique_ptr<UiSprite> sprGetTotalOption;
	std::unique_ptr<UiSprite> sprNowWave;
	std::unique_ptr<UiSprite> sprTotalHitDamage;

	static float highScore;
	bool isHighScore = false;

	// ���ꂼ��̕\���̊J�n����
	float startHightScore = 5.8f;

	float startRank = 6.0f;

	float startTotalScore = 4.8f;
	float startAliveTime = 3.5f;

	float startTotalDestoryEnemy = 2.4f;
	float startTotalGetOption = 2.6f;
	float startNowWave = 2.8f;
	float startTotalHitDamage = 3.0f;

	// �����N�Ή��X�R�A
	float rankB = 3000;
	float rankA = 6000;
	float rankS = 12000;
};