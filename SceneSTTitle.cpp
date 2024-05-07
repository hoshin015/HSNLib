#include "SceneSTTitle.h"
#include "SceneManager.h"
#include "Library/Input/InputManager.h"
#include "Library/ImGui/Include/imgui.h"
#include "Library/Text/DispString.h"
#include "Library/Text/Text.h"
#include "Library/Timer.h"
#include "Library/Input/InputManager.h"
#include "Library/Framework.h"

#include "Video.h"


#include <tchar.h>
#include <filesystem>

void SceneSTTitle::Initialize() {
	VideoManager::Instance().LoadFile(0, nullptr);
	VideoManager::Instance().Play(0, true);
	titleSprite = std::make_unique<Sprite>(L"Data/Texture/tyu-.png");

}

void SceneSTTitle::Finalize() {
	VideoManager::Instance().Clear();
}

void SceneSTTitle::Update() {
	InputManager& im = InputManager::Instance();
	VideoManager::Instance().Update();

	if (im.GetKeyPressed(Keyboard::Enter)) {
		SceneManager::Instance().ChangeScene(new SceneLoading(new SceneSTMainMenu));
	}
}

void SceneSTTitle::Render() {
	float width = Framework::Instance().windowWidth;
	float height = Framework::Instance().windowHeight;
	float wRatio = width / 1280;
	float hRatio = height / 720;
	float sRatio = wRatio * hRatio;

	VideoManager::Instance().Draw(0, {0,0}, {width,height}, _videoColor);
	DispString::Instance().Draw(L"～Press Enter～", { width * .5f, height * .8f }, 25 * sRatio, TEXT_ALIGN::MIDDLE, { 0, 0, 0, 1 }, true, { 1,1,1,.5f });
	DispString::Instance().Draw(L"ESC Quit", { width * .01f, height * .99f }, 25 * sRatio, TEXT_ALIGN::LOWER_LEFT, { 0, 0, 0, 1 }, true, { 1,1,1,.5f });
	titleSprite->Render(0, 0, wRatio * 300, hRatio * 200, 1, 1, 1, 1, 0);
	fadeout.Render(0, 0, width, height, 0, 0, 0, fadeoutA, 0);

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

		ImGui::ColorEdit4("Color", &_videoColor.x);

		static bool isLoop = false;
		ImGui::Checkbox("Loop", &isLoop);
		if (ImGui::Button("Play")) VideoManager::Instance().Play(0, isLoop);
		if (ImGui::Button("Pause")||InputManager::Instance().GetKeyPressed(Keyboard::Space)) VideoManager::Instance().Pause(0);
		if (ImGui::Button("Stop")) VideoManager::Instance().Stop(0);

		static float timeSec = 0;
		ImGui::DragFloat("SeekTime", &timeSec);
		if (ImGui::Button("Seek"))VideoManager::Instance().SeekPosition(0, timeSec);
	}
	ImGui::End();
}
