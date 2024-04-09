#pragma once

#include "Character.h"
#include "ProjectileManager.h"

enum class PlayerAnim
{
	Attack01,
	Attack02,
	Attack03,
	IdleBattle,
	JumpAirNormal,
	MoveFwdBattle,
	Victory
};

// プレイヤー
class Player : public Character
{
public:
	Player();
	~Player() override;

	// 更新処理
	void Update();

	// 描画処理
	void Render();

	// デバッグ用GUI描画
	void DrawDebugGui();

private:
	
	// 移動入力処理
	bool InputMove();
	// スティック入力値から移動ベクトルを取得
	DirectX::XMFLOAT3 GetMoveVec() const;
	// ジャンプ入力処理
	bool InputJump();
	// 攻撃入力処理
	bool InputAttack();

	// 着地処理
	void OnLanding() override;

	// 弾丸入力処理
	void InputProjectile();
	
	//--------------------------------------------------------------
	//  衝突判定
	//--------------------------------------------------------------
	
	// プレイヤーとエネミーのくらい判定の衝突
	void CollisionBodyPlayerVsEnemies();

	// プレイヤーとエネミーの衝突処理
	void CollisionPlayerVsEnemies();
	// 弾丸と敵の衝突処理
	void CollisionProjectilesVsEnemies();
	// アニメーションとエネミーの衝突処理
	void CollisionNodeVsEnemies();

	
	
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

	// ジャンプステートへ遷移
	void TransitionJumpState();
	// ジャンプステート更新処理
	void UpdateJumpState();

	// 攻撃ステートへ遷移
	void TransitionAttackState();
	// 攻撃ステート更新処理
	void UpdateAttackState();

	// 勝利ステートへ遷移
	void TransitionVictoryState();
	// 勝利ステート更新処理
	void UpdateVictoryUpdate();

private:

	ProjectileManager projectileManager;

	float moveSpeed = 5.0f;
	float turnSpeed = 720;
	float jumpSpeed = 20.0f;
	int jumpCount = 0;
	int jumpLimit = 2;

	float radius = 0.4f;
	float height = 1.4f;
	

	// --- ステート管理 ---
	enum class State
	{
		Idle,
		Move,
		Jump,
		Land,
		Attack,
		Victory,
	};
	State state = State::Idle;
};