#include "LightManager.h"
#include "Library/ImGui/Include/imgui.h"

LightManager::~LightManager()
{
	Clear();
}

// ライト登録
void LightManager::Register(Light* light)
{
	lights.emplace_back(light);
}

// ライト削除
void LightManager::Remove(Light* light)
{
	std::vector<Light*>::iterator	it = std::find(lights.begin(), lights.end(), light);
	if (it != lights.end())
	{
		lights.erase(it);
		delete	light;
	}
}

// ライト全削除
void LightManager::Clear()
{
	for (Light* light : lights)
	{
		delete	light;
	}
	lights.clear();

}

// ライト情報を積む
void LightManager::PushLightData(Graphics::SceneConstants& cBuffer)
{
	// 環境光の情報を追加
	cBuffer.ambientLightColor = ambientColor;

	// 登録されている光源の情報を設定
	for (Light* light : lights)
	{
		light->PushLightData(cBuffer);
	}
}

// デバッグ情報の表示
void LightManager::DrawDebugGui()
{
	if (ImGui::TreeNode("Lights"))
	{
		ImGui::ColorEdit3("AmbientColor", &ambientColor.x);
		int nodeId = 0;
		for (Light* light : lights)
		{
			ImGui::PushID(nodeId++);
			light->DrawDebugGUI();
			ImGui::PopID();
		}
		ImGui::TreePop();
	}

}

// デバッグプリミティブ描画
void LightManager::DrawDebugPrimitive()
{
	for (Light* light : lights)
	{
		light->DrawDebugPrimitive();
	}
}


