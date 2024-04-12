#include <tchar.h>
#include "SceneTest.h"
#include "SceneManager.h"
#include "Library/Input/InputManager.h"
#include "Library/ImGui/Include/imgui.h"
#include "Library/Timer.h"

void SceneTest::Initialize()
{
}

void SceneTest::Finalize()
{
}

void SceneTest::Update()
{
	InputManager& input = InputManager::Instance();

	if (input.GetKeyPressed(Keyboard::Enter))
	{
		SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
	}
}

void SceneTest::Render()
{
	

	// --- デバッグ描画 ---
	DrawDebugGUI();
}

// デバッグ描画
void SceneTest::DrawDebugGUI()
{
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::BeginMenu("Scene")) {
				if (ImGui::MenuItem("Title"))
				{
					SceneManager::Instance().ChangeScene(new SceneTitle);
				}
				if (ImGui::MenuItem("Game"))
				{
					SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
				}
				if (ImGui::MenuItem("Animation"))
				{
					SceneManager::Instance().ChangeScene(new SceneAnimation);
				}
				if (ImGui::MenuItem("ContextBase"))
				{
					SceneManager::Instance().ChangeScene(new SceneLoading(new SceneContextBase));
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}
