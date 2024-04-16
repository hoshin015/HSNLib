#include "SpinningTopEnemyManager.h"
#include "Library/ImGui/Include/imgui.h"
#include "Collision.h"
#include "EnemySlime.h"

// 更新処理
void SpinningTopEnemyManager::Update()
{
	// 更新処理
	for (SpinningTopEnemy* enemy : enemies)
	{
		enemy->Update();
	}

	// 破棄処理
	for (SpinningTopEnemy* enemy : removes)
	{
		std::vector<SpinningTopEnemy*>::iterator it = std::find(enemies.begin(), enemies.end(), enemy);

		if (it != enemies.end())
		{
			enemies.erase(it);
		}
		// 弾丸の破棄処理
		delete enemy;
	}
	// 破棄リストをクリア
	removes.clear();
}

//　描画処理
void SpinningTopEnemyManager::Render()
{
	for (SpinningTopEnemy* enemy : enemies)
	{
		enemy->Render();
	}

	// 敵同士の衝突処理
	CollisionEnemyVsEnemeis();
}

// エネミー登録
void SpinningTopEnemyManager::Register(SpinningTopEnemy* enemy)
{
	enemies.emplace_back(enemy);
	enemy->id = enemies.size() - 1;
}


// エネミー削除
void SpinningTopEnemyManager::Remove(SpinningTopEnemy* enemy)
{
	// 破棄リストに追加
	removes.insert(enemy);
}

// エネミー全削除
void SpinningTopEnemyManager::Clear()
{
	for (SpinningTopEnemy* enemy : enemies)
	{
		delete enemy;
	}
	enemies.clear();
}

void SpinningTopEnemyManager::SortPlaySortEnemiesByDistanceToPlayerEnemy(DirectX::XMFLOAT3 playerPosition)
{
	// 全てのenemyでplayerとの距離を計測
	DirectX::XMVECTOR playerPos = DirectX::XMLoadFloat3(&playerPosition);
	for (SpinningTopEnemy* enemy : enemies)
	{
		DirectX::XMVECTOR enemyPos = DirectX::XMLoadFloat3(&enemy->GetPosition());

		DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(enemyPos, playerPos);
		DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);
		DirectX::XMStoreFloat(&enemy->playerDistance, LengthSq);
	}

	int enemyCount = enemies.size();
	for (int i = 0; i < enemyCount; i++)
	{
		SpinningTopEnemy* enemyA = enemies.at(i);

		// 全てのenemyと比較
		for (int j = 0; j < enemyCount; j++)
		{
			SpinningTopEnemy* enemyB = enemies.at(j);

			if (enemyA == enemyB) continue;

			if (enemyA->playerDistance < enemyB->playerDistance)
			{
				std::swap(enemies[i], enemies[j]);
			}
		}
	}
}

bool SpinningTopEnemyManager::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	bool result = false;

	hit.distance = FLT_MAX;
	for (SpinningTopEnemy* enemy : enemies)
	{
		HitResult hitResult;
		if (enemy->RayCast(start, end, hitResult))
		{
			if (hit.distance > hitResult.distance)
			{
				hit = hitResult;
				result = true;
			}
		}
	}

	return result;
}

bool SpinningTopEnemyManager::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit, int id)
{
	bool result = false;

	hit.distance = FLT_MAX;
	for (SpinningTopEnemy* enemy : enemies)
	{
		if (enemy->id == id) continue;

		HitResult hitResult;
		if (enemy->RayCast(start, end, hitResult))
		{
			if (hit.distance > hitResult.distance)
			{
				hit = hitResult;
				result = true;
			}
		}
	}

	return result;
}

// エネミー同士の衝突判定
void SpinningTopEnemyManager::CollisionEnemyVsEnemeis()
{
	for (SpinningTopEnemy* enemyA : enemies)
	{
		for (SpinningTopEnemy* enemyB : enemies)
		{
			if (enemyA == enemyB) continue;

			// 衝突処理
			DirectX::XMFLOAT3 outPosition;
			if (Collision::IntersectCylinderVsCylinder(
				enemyA->GetPosition(),
				enemyA->GetRadius(),
				enemyA->GetHeight(),
				enemyB->GetPosition(),
				enemyB->GetRadius(),
				enemyB->GetHeight(),
				outPosition)
				)
			{
				// 押し出し後の位置設定
				enemyB->SetPosition(outPosition);
			}
		}
	}

}


// デバッグ用GUI描画
void SpinningTopEnemyManager::DrawDebugGui()
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("SpinningTopEnemyManager", nullptr, ImGuiWindowFlags_None))
	{
		uint32_t enemyCount = enemies.size();
		for (int i = 0; i < enemyCount; i++)
		{
			SpinningTopEnemy* enemy = enemies.at(i);
			if (ImGui::CollapsingHeader(("SpinningTopEnemy - " + std::to_string(i)).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				// 位置
				DirectX::XMFLOAT3 position = enemy->GetPosition();
				ImGui::DragFloat3("Position", &position.x, 0.1f);
				ImGui::InputFloat("Distance", &enemy->playerDistance);
				ImGui::DragFloat3("Target", &enemy->targetPosition.x, 0.1f);
				ImGui::DragFloat("rotationSpeed", &enemy->rotationSpeed, 0.1f);
			}
		}
	}
	ImGui::End();
}