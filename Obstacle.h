#pragma once
#include "Library/3D/SkinnedMesh.h"
#include "Collision.h"

class Obstacle
{
public:
	Obstacle() {}
	virtual ~Obstacle() {};

	// 更新処理
	virtual void Update() {}

	// 描画処理
	void Render();

	// デバッグプリミティブ描画
	void DrawDebugPrimitive();


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

	// 半径
	float GetRadius() { return radius; }
	void SetRadius(float radius) { this->radius = radius; }

	// 回転角度
	float GeRotationSpeed() { return rotationSpeed; }
	void SetRotationSpeed(float rotationSpeed) { this->rotationSpeed = rotationSpeed; }

	// 抵抗
	float GetFriction() { return frictionPower; }
	void SetFriction(float friction) { this->frictionPower = friction; }

	// レイキャスト
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);
protected:
	// 行列更新関数
	void UpdateTransform();

public:
	std::shared_ptr<SkinnedMesh> model;

	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT3 angle = { 0,0,0 };
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	DirectX::XMFLOAT4X4 transform =
	{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};

	DirectX::XMFLOAT3 targetPosition = { -1,-1,-1 };
	float moveSpeed = 1.0f;

	DirectX::XMFLOAT3 velocity = { 0,0,0 };
	float frictionPower = 5.0f;

	float rotationSpeed = 0;	// 1秒に回転する角度

	float radius = 1.1f;

	bool isCollision = true;

	int hitDamae = 0;	// プレイヤーと衝突したときに与えるダメージ
};