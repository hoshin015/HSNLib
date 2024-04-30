#include "SceneSTTitle.h"
#include "SceneManager.h"
#include "Library/Input/InputManager.h"
#include "Library/ImGui/Include/imgui.h"
#include "Library/Text/DispString.h"
#include "Library/Text/Text.h"
#include "Library/Timer.h"

#include "Video.h"

#include <tchar.h>
#include <filesystem>

void SceneSTTitle::Initialize() {
	VideoManager::Instance().LoadFile(0, L"C:/Users/2230137/Videos/Captures/PlatformRunner.mp4");
}

void SceneSTTitle::Finalize() {
	VideoManager::Instance().Clear();
}

void SceneSTTitle::Update() {
	VideoManager::Instance().Play(0, true);
	VideoManager::Instance().Update();
}

void SceneSTTitle::Render() {
	VideoManager::Instance().Draw(0, _videoPos, _videoSize, _videoColor);
	DrawDebugGUI();
}

void SceneSTTitle::DrawDebugGUI() {
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::BeginMenu("Scene")) {
				if (ImGui::MenuItem("Title")) {
					SceneManager::Instance().ChangeScene(new SceneTitle);
				}
				if (ImGui::MenuItem("Game")) {
					SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
				}
				if (ImGui::MenuItem("Animation")) {
					SceneManager::Instance().ChangeScene(new SceneAnimation);
				}
				if (ImGui::MenuItem("ContextBase")) {
					SceneManager::Instance().ChangeScene(new SceneLoading(new SceneContextBase));
				}
				if (ImGui::MenuItem("Test")) {
					SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTest));
				}
				if (ImGui::MenuItem("SpinningTop")) {
					SceneManager::Instance().ChangeScene(new SceneLoading(new SceneSpinningTop));
				}
				if (ImGui::MenuItem("STPlayer")) {
					SceneManager::Instance().ChangeScene(new SceneLoading(new SceneSTPlayer));
				}
				if (ImGui::MenuItem("EnemyLevelEditor")) {
					SceneManager::Instance().ChangeScene(new SceneLoading(new SceneEnemyLevelEditor));
				}
				if (ImGui::MenuItem("SpawnEditor")) {
					SceneManager::Instance().ChangeScene(new SceneLoading(new SceneSpawnEditor));
				}
				if (ImGui::MenuItem("SceneWave")) {
					SceneManager::Instance().ChangeScene(new SceneLoading(new SceneWave));
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	if (ImGui::Begin("Video")) {
		static std::wstring videofile;
		static std::string sfile;
		if (ImGui::Button("Open Files...")) {
			OPENFILENAME ofn;       // ファイル選択用の構造体
			TCHAR szFile[260] = { 0 };  // ファイルパスを格納するバッファ

			// 構造体の初期化
			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lpstrFilter = _TEXT("mp4ファイル(*.mp4)\0*.mp4\0") _TEXT("全てのファイル(*.*)\0*.*\0");
			ofn.lStructSize = sizeof(ofn);
			ofn.lpstrFile = szFile;
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = sizeof(szFile);
			ofn.nFilterIndex = 1;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

			GetOpenFileName(&ofn);

			// --- filesystem によるpath変換 ---

			std::filesystem::path selectedPath(szFile);

			sfile = selectedPath.string();
			videofile = selectedPath.c_str();

		}
		ImGui::InputText("VideoPath", (char*)sfile.data(), sizeof(sfile));

		if(ImGui::Button("Load Video")) {
			VideoManager::Instance().Stop(0);
			VideoManager::Instance().Clear(0);
			VideoManager::Instance().LoadFile(0, videofile.data());
		}

		ImGui::DragFloat2("Pos", &_videoPos.x);
		ImGui::DragFloat2("Size", &_videoSize.x);
		ImGui::ColorEdit4("Color", &_videoColor.x);
	}
	ImGui::End();
}
