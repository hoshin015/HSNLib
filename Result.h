#pragma once
#include "UiSprite.h"

class Result
{
public:
	Result();
	~Result() {};

	// 更新
	void Update();

	// 描画
	void Render();

	// スコア計算
	void CalcScore();

	// --- リザルト画面 ---
	bool isResult = false;
	bool resultFinish = false;
	float resultFinishTime = 6.0f;
	float waitTime = 2.0f;		// プレイヤー死亡からの待ち時間
	float waitTimer = 0.0f;		// プレイヤー死亡からすぐにUI表示をしないために使う
	std::unique_ptr<UiSprite> sprMainMenu;
	std::unique_ptr<UiSprite> sprSideBar1;
	std::unique_ptr<UiSprite> sprSideBar2;

	std::unique_ptr<UiSprite> sprRankS;

	// テキスト
	std::unique_ptr<UiSprite> sprTotalScore;
	std::unique_ptr<UiSprite> sprAliveTime;

	std::unique_ptr<UiSprite> sprTotalDestoryEnemy;
	std::unique_ptr<UiSprite> sprGetTotalOption;
	std::unique_ptr<UiSprite> sprNowWave;
	std::unique_ptr<UiSprite> sprTotalHitDamage;


	// それぞれの表示の開始時間
	float startRank = 5.0f;

	float startTotalScore = 4.0f;
	float startAliveTime = 4.0f;

	float startTotalDestoryEnemy = 2.0f;
	float startTotalGetOption = 2.3f;
	float startNowWave = 2.6f;
	float startTotalHitDamage = 2.9f;
};