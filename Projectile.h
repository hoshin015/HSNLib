#pragma once

#include "Library/Graphics/Graphics.h"
#include "Library/3D/SpherePrimitive.h"

// 前方宣言
class ProjectileManager;

// 弾丸
class Projectile
{
public:
	Projectile(ProjectileManager* manager);
	virtual ~Projectile() {}

	// 更新処理
	virtual void Update() = 0;

	// 描画処理
	virtual void Render() = 0;

	// 位置取得
	const DirectX::XMFLOAT3& GetPosition() const { return position; }
	// 方向取得
	const DirectX::XMFLOAT3& GetDirection() const { return direction; }
	// スケール取得
	const DirectX::XMFLOAT3& GetScale() const { return scale; }
	// 半径取得
	float GetRadius() const { return radius; }

	// 破棄
	void Destroy();

protected:
	// 行列更新処理
	void UpdateTransform();

protected:
	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT3 direction = { 0,0,1 };
	DirectX::XMFLOAT3 scale = { 1,1,1 };
	DirectX::XMFLOAT4X4 transform =
	{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};

	ProjectileManager* manager = nullptr;

	SpherePrimitive* sphere = nullptr;
	float radius = 0.3f;
};