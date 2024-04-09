#pragma once

#include <vector>
#include <set>
#include "Enemy.h"

// エネミーマネージャー
class EnemyManager
{
private:
	EnemyManager() {}
	~EnemyManager() {}

public:
	// 唯一のインスタンス取得
	static EnemyManager& Instance()
	{
		static EnemyManager instance;
		return instance;
	}

	// 更新処理
	void Update();

	//　描画処理
	void Render();

	// エネミー登録
	void Register(Enemy* enemy);

	// エネミー削除
	void Remove(Enemy* enemy);

	// エネミー全削除
	void Clear();

	// エネミー数取得
	int GetEnemyCount() const { return static_cast<int>(enemies.size()); }

	// エネミー取得
	Enemy* GetEnemy(int index) { return enemies.at(index); }

	// デバッグ用GUI描画
	void DrawDebugGui();

	// エネミーをプレイヤーに近い順にソートする
	void SortPlaySortEnemiesByDistanceToPlayerEnemy(DirectX::XMFLOAT3 playerPosition);

	// レイキャスト
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit, int id);

private:
	// エネミー同士の衝突判定
	void CollisionEnemyVsEnemeis();

private:
	std::vector<Enemy*> enemies;
	std::set<Enemy*> removes;
};