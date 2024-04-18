#pragma once
#include "Library/3D/SkinnedMesh.h"

#include "Collision.h"

class Obstacle
{
public:
	Obstacle(const char* name);
	~Obstacle();

	// 更新処理
	void Update();

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

	float GetRadius() { return radius; }

	// レイキャスト
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);
	


private:
	// 行列更新関数
	void UpdateTransform();

public:
	SkinnedMesh* model = nullptr;

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

	float radius = 1.1f;
};