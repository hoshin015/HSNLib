#include "StageManager.h"

// 更新処理
void StageManager::Update()
{
	for (Stage* stage : stages)
	{
		stage->Update();
	}
}

// 描画処理
void StageManager::Render()
{
	for (Stage* stage : stages)
	{
		stage->Render();
	}
}

// ステージ登録
void StageManager::Register(Stage* stage)
{
	stages.emplace_back(stage);
}

// ステージ全削除
void StageManager::Clear()
{
	stages.clear();
}

// レイキャスト
bool StageManager::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	bool result = false;

	hit.distance = FLT_MAX;
	for (Stage* stage : stages)
	{
		HitResult hitResult;
		if (stage->RayCast(start, end, hitResult))
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

