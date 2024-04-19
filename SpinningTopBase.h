#pragma once
#include <DirectXMath.h>
#include <memory>
#include "Library/3D/SkinnedMesh.h"
#include "Library/Effekseer/Effect.h"

class SpinningTopBase
{
public:
	SpinningTopBase() {}
	virtual ~SpinningTopBase() {}

	// 行列更新関数
	void UpdateTransform();

	// 行列取得
	const DirectX::XMFLOAT4X4& GetTransform() const { return transform; }

	// 位置取得
	const DirectX::XMFLOAT3& GetPosition() const { return position; }
	// 位置設定
	void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }
	// 回転取得
	const DirectX::XMFLOAT3& GetAngle() const { return angle; }
	// 回転設定
	void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }
	// スケール取得
	const DirectX::XMFLOAT3& GetScale() const { return scale; }
	// スケール設定
	void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }
	// 半径設定
	void SetRadius(const float radius) { this->radius = radius; }

	// velocity取得
	DirectX::XMFLOAT3 GetVelocity() { return this->velocity; }
	// velocity設定
	void SetVelocity(const DirectX::XMFLOAT3& velocity) { this->velocity = velocity; }

	// 接地判定取得
	bool IsGround() const { return isGround; }

	// 半径取得
	float GetRadius() const { return radius; }
	// 高さ取得
	float GetHeight() const { return height; }

	// 健康状態を取得
	int GetHealth() const { return health; }
	// 最大健康状態を取得
	int GetMaxHealth() const { return maxHealth; }



	// ダメージを与える
	bool ApplyDamage(int damage, float invincibleTime);
	// 衝撃を与える
	void AddImpulse(const DirectX::XMFLOAT3& impulse);

protected:
	// 移動処理
	void Move(float vx, float vz, float speed);
	// 旋回処理
	void Turn(float vx, float vz, float speed);
	// ジャンプ処理
	void Jump(float speed);
	// 速力更新処理
	void UpdateVelocity();

	// 着地処理
	virtual void OnLanding() {}

	// ダメージ処理
	virtual void OnDamaged() {}
	// 死亡処理
	virtual void OnDead() {}

	// 無敵時間処理
	void UpdateInvincibleTimer();



private:
	// 垂直速力更新処理
	void UpdateVerticalVelocity();
	// 垂直移動更新処理
	void UpdateVerticalMove();
	// 水平速力更新処理
	void UpdateHorizontalVelocity();
	// 水平移動更新処理
	void UpdateHorizontalMove();

protected:
	//--------------------------------------------------------------
	//  エフェクト
	//--------------------------------------------------------------

	// エフェクト更新処理
	void UpdateEffect();
	// エフェクトフラグクリア
	void ClearEffectFlag();

	//--------------------------------------------------------------
	//  効果音
	//--------------------------------------------------------------

	// 効果音更新処理
	void UpdateSE();
	// 効果音フラグクリア
	void ClearSEFlag();

	//--------------------------------------------------------------
	//  アニメーション
	//--------------------------------------------------------------

	// アニメーション再生
	void PlayAnimation(int index, bool loop);
	// アニメーション更新
	void UpdateAnimation();

	//--------------------------------------------------------------
	//  デバッグ描画
	//--------------------------------------------------------------
	void DrawDebugPrimitive();

protected:
	DirectX::XMFLOAT3		position = { 0,0,0 };
	DirectX::XMFLOAT3		angle = { 0,0,0 };
	DirectX::XMFLOAT3		scale = { 1,1,1 };
	DirectX::XMFLOAT4X4		transform =
	{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
	DirectX::XMFLOAT3 velocity = { 0,0,0 };
	float gravity = -1.0f;
	bool isGround = false;

	float radius = 0.5f;
	float height = 1.0f;

	int health = 5;
	int maxHealth = 5;
	float invincibleTimer = 1.0f;

	float friction = 0.25f;
	float airControl = 0.3f;

	float acceleration = 1.0f;
	float maxMoveSpeed = 15.0f;
	float moveVecX = 0.0f;
	float moveVecZ = 0.0f;

	// RayCastのオフセット
	float stepOffset = 0.5f;
	float stepOffset1 = 0.1f;
	float slopeRate = 1.0f;

public:
	std::shared_ptr<SkinnedMesh>	model;
	std::shared_ptr<SkinnedMesh>	paryEffect;

	// --- アニメーションフラグ ---
	Animation::KeyFrame keyFrame;			// 使用されるキーフレーム
	int currentKeyFrame = 0;
	int currentAnimationIndex = 0;			// 現在再生中のアニメーション番号
	float currentAnimationSeconds = 0.0f;	// 現在再生中の秒数

	bool animationLoopFlag = true;			// ループ再生するかどうか
	bool animationEndFlag = false;			// アニメーションが最後のキーフレームに到達したかどうか

	float animationBlendTime = 0.0f;
	float animationBlendSeconds = 0.0f;
	int blendAnimationIndex = -1;			// 遷移前のアニメーション番号
};