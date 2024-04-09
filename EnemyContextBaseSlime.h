#pragma once

#include "Enemy.h"

enum class EnemyContextBaseSlimeAnim
{
	Attack01,
	Attack02,
	Die,
	Dizzy,
	GetHit,
	IdleBattle,
	IdleNormal,
	RunFWD,
	SenseSomethingPRT,
	SemseSomethingST,
	Taunt,
	Victory,
	WalkBWD,
	WalkFWD,
	WalkLeft,
	WalkRight
};


class EnemyContextBaseSlime : public Enemy
{
public:
	EnemyContextBaseSlime();
	~EnemyContextBaseSlime() override;

	// 更新処理
	void Update() override;

	// 描画処理
	void Render() override;

	// デバッグ用GUI描画
	void DrawDebugGui();

	// ContextBase 描画
	void DrawContext();


protected:
	// 死亡処理
	void OnDead() override;

	//--------------------------------------------------------------
	//  ステート管理
	//--------------------------------------------------------------

	// 待機ステートへ遷移
	void TransitionIdleState();
	// 待機ステート更新処理
	void UpdateIdleState();

	// 移動ステートへ遷移
	void TransitionMoveState();
	// 移動ステート更新処理
	void UpdateMoveState();

	//--------------------------------------------------------------
	//  ContextBase
	//--------------------------------------------------------------

	// プレイヤーへのレイキャスト
	void RayTarget();

	// 角度計算
	void CalcOneRadius();
	// 興味チェック
	void InterestCheck();
	// 危険チェック
	void AccelerationCheck();
	// 最終的な移動ベクトル算出
	void CalcContextVec();

	// 追跡終了チェック
	void EndChaseCheck();


private:
	float radius = 0.5f;
	float height = 1.0f;


	// --- ステート管理 ---
	enum class State
	{
		Idle,
		Move,
		Attack,
		Hit,
		DIe
	};
	State state = State::Idle;

	// --- ContextBase ---

	static const int numRays = 12;		// レイの本数
	float lookAhead = 5;				// レイの距離

	float rayDirections[numRays] = {};	// レイの角度
	float interest[numRays] = {};		// 対応する角度にどれだけ移動したいか
	float danger[numRays] = {};			// 対応する角度にどれだけ移動したくないか
		
	float targetMinDistance = 2.0f;		// この距離までしか近づかない
	float targetMaxDistance = 5.0f;		// この距離からは徐々に使づきたい意欲減る

	DirectX::XMFLOAT3 contextVelocity = { 0,0,0 };		// 最終的に移動するベクトル
		
	DirectX::XMFLOAT3 lastTargetPosition = { 0,0,0 };	// 保存しておくターゲットの位置
};