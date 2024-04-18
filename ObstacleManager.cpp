#include "ObstacleManager.h"
#include "Library/ImGui/Include/imgui.h"
#include "Collision.h"

// 更新処理
void ObstacleManager::Update()
{
	// 更新処理
	for (Obstacle* obstacle : obstacles)
	{
		obstacle->Update();
	}

	// 破棄処理
	for (Obstacle* obstacle : removes)
	{
		std::vector<Obstacle*>::iterator it = std::find(obstacles.begin(), obstacles.end(), obstacle);

		if (it != obstacles.end())
		{
			obstacles.erase(it);
		}
		// 弾丸の破棄処理
		delete obstacle;
	}
	// 破棄リストをクリア
	removes.clear();
}

//　描画処理
void ObstacleManager::Render()
{
	for (Obstacle* obstacle : obstacles)
	{
		obstacle->Render();
		obstacle->DrawDebugPrimitive();
	}
}



// エネミー登録
void ObstacleManager::Register(Obstacle* obstacle)
{
	obstacles.emplace_back(obstacle);
}


// エネミー削除
void ObstacleManager::Remove(Obstacle* obstacle)
{
	// 破棄リストに追加
	removes.insert(obstacle);
}

// エネミー全削除
void ObstacleManager::Clear()
{
	for (Obstacle* obstacle : obstacles)
	{
		delete obstacle;
	}
	obstacles.clear();
}

// デバッグ用GUI描画
void ObstacleManager::DrawDebugGui()
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("ObstacleManager", nullptr, ImGuiWindowFlags_None))
	{
		uint32_t obstacleCount = obstacles.size();
		for (int i = 0; i < obstacleCount; i++)
		{
			Obstacle* obstacle = obstacles.at(i);
			if (ImGui::CollapsingHeader(("Obstacle - " + std::to_string(i)).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				// 位置
				DirectX::XMFLOAT3 position = obstacle->GetPosition();
				ImGui::DragFloat3("Position", &position.x, 0.1f);
			}
		}
	}
	ImGui::End();
}