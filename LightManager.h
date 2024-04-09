#pragma once

#include <vector>
#include "Light.h"

// ライトマネージャー
class LightManager
{
private:
	LightManager() {}
	~LightManager();

public:
	// 唯一のインスタンス取得
	static LightManager& Instance()
	{
		static LightManager instance;
		return instance;
	}

	// ライト登録
	void Register(Light* light);

	// ライト削除
	void Remove(Light* light);

	// ライト全削除
	void Clear();

	// ライト情報を積む
	void PushLightData(Graphics::SceneConstants& cBuffer);


	// デバッグ情報の表示
	void DrawDebugGui();

	// デバッグプリミティブ描画
	void DrawDebugPrimitive();

	// 環境光
	DirectX::XMFLOAT4 GetAmbientColor() { return ambientColor; }
	void SetAmbientColor(DirectX::XMFLOAT4 ambientColor) { this->ambientColor = ambientColor; }

	// ライトの情報
	int GetLightCount() const { return static_cast<int>(lights.size()); }
	Light* GetLight(int index) const { return lights.at(index); }

private:
	std::vector<Light*> lights;
	DirectX::XMFLOAT4 ambientColor = { 0.2f, 0.2f, 0.2f, 1.0f };
};