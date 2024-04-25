#pragma once
#include <DirectXMath.h>
#include <memory>
#include "Library/3D/SkinnedMesh.h"
#include "Library/Effekseer/Effect.h"

class MeshEffect
{
public:
	MeshEffect() {}
	virtual ~MeshEffect() {}

	// 更新処理
	virtual void Update() = 0;

	// 描画処理
	virtual void Render() = 0;

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


	// 半径取得
	float GetRadius() const { return radius; }
	// 高さ取得
	float GetHeight() const { return height; }

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

	float radius = 0.5f;
	float height = 1.0f;

public:
	std::shared_ptr<SkinnedMesh>	model;
};