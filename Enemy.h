#pragma once

#include "Character.h"
#include "Library/3D/SkinnedMesh.h"
#include "Collision.h"

// エネミー
class Enemy : public Character
{
public:
	Enemy() {}
	~Enemy() override {}

	// 更新処理
	virtual void Update() = 0;

	// 描画処理
	virtual void Render() = 0;

	// レイキャスト
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);

	// 破棄
	void Destroy();

public:
	// enemies での index
	int id;

	// プレイヤーとの距離
	float playerDistance;
};