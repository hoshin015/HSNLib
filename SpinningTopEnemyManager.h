#pragma once
#pragma once

#include <vector>
#include <set>
#include "SpinningTopEnemy.h"
#include "StEnemyData.h"

// エネミーマネージャー
class SpinningTopEnemyManager
{
private:
	SpinningTopEnemyManager() {}
	~SpinningTopEnemyManager() {}

public:
	// 唯一のインスタンス取得
	static SpinningTopEnemyManager& Instance()
	{
		static SpinningTopEnemyManager instance;
		return instance;
	}

	// 更新処理
	void Update();

	//　描画処理
	void Render(bool drawShadow = false);

	// エネミー登録
	void Register(SpinningTopEnemy* enemy);

	// エネミー削除
	void Remove(SpinningTopEnemy* enemy);

	// エネミー全削除
	void Clear();

	// エネミー数取得
	int GetEnemyCount() const { return static_cast<int>(enemies.size()); }

	// エネミー取得
	SpinningTopEnemy* GetEnemy(int index) { return enemies.at(index); }

	// デバッグ用GUI描画
	void DrawDebugGui();

	// エネミーをプレイヤーに近い順にソートする
	void SortPlaySortEnemiesByDistanceToPlayerEnemy(DirectX::XMFLOAT3 playerPosition);

	// レイキャスト
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit, int id);

	// エネミーのステータス値を更新
	void UpdateStatusValue(EnemyData* pData);

private:
	// エネミー同士の衝突判定
	void CollisionEnemyVsEnemeis();

	// エネミーと障害物の衝突判定
	void CollisionEnemyVsObstacle();

public:
	bool drawDebugPrimitive = true;

private:
	std::vector<SpinningTopEnemy*> enemies;
	std::set<SpinningTopEnemy*> removes;
};