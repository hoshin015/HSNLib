
#include "../Graphics/Graphics.h"
#include "../Timer.h"
#include "ImGuiManager.h"
#include "ConsoleData.h"


// 初期化
void ImGuiManager::Initialize(HWND hwnd)
{
	// ImGuiの準備
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	// Dockingの有効化
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	// 画面外対応
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	// ウィンドウの深度変更対応
	io.ConfigViewportsNoDefaultParent = true;


	// Font
	io.Fonts->AddFontFromFileTTF("Data/Font/meiryob.ttc", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());

	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(Graphics::Instance().device.Get(), Graphics::Instance().deviceContext.Get());
	
	// スタイルの設定
	ImGui::StyleColorsDark();

}

// 終了化
void ImGuiManager::Finalize()
{
	// --- ImGuiの終了 ---
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

// 更新
void ImGuiManager::Update()
{
	// --- ImGuiの開始 ---
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// --- DockSpace 処理 ---
	DockSpace();
}

// 描画
void ImGuiManager::Render()
{
	// --- Console描画 ---
	//Console();

	// --- ImGui描画 ---
	ImGui::Render();	// 描画データの組み立て
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}



// DockSpace処理
void ImGuiManager::DockSpace()
{
	// https://gist.github.com/moebiussurfing/8dbc7fef5964adcd29428943b78e45d2

	// --- windowフラグ設定 ---
	static ImGuiWindowFlags windowFlags = 
		//ImGuiWindowFlags_MenuBar |		// これを有効にするとメニューバーを追加できる
		ImGuiDockNodeFlags_PassthruCentralNode |
		ImGuiWindowFlags_NoDocking |
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse  | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;


	// --- dockNodeフラグ設定 ---
	const ImGuiDockNodeFlags dockFlags = ImGuiDockNodeFlags_PassthruCentralNode;

	// --- ImGuiの表示設定 ---
	ImGuiViewport* viewport = ImGui::GetMainViewport();	// メインビューの値を取得
	ImGui::SetNextWindowPos(viewport->WorkPos);			// ポジションの設定
	ImGui::SetNextWindowSize(viewport->Size, ImGuiCond_Always);		// サイズの設定
	ImGui::SetNextWindowViewport(viewport->ID);			// IDの設定
	ImGui::SetNextWindowBgAlpha(0.0f);					// 背景アルファの設定

	// --- DockSpaceの周囲スタイルの設定? ---
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	// --- DockSpaceの作成 ---
	ImGui::Begin("HSNLib", NULL, windowFlags);
	ImGui::PopStyleVar(3);	// 周囲スタイルの適用?

	ImGuiID dockSpaceID = ImGui::GetID("MyDockSpace");
	ImGui::DockSpace(dockSpaceID, ImVec2(0.f, 0.f), dockFlags);

	ImGui::End();
}

// Console処理
void ImGuiManager::Console()
{
	ImGui::Begin("Console");
	{
		if (ImGui::Button("Clear", ImVec2(60, 20)))
		{
			ConsoleData::Instance().logs.clear();
		}

		// テキストを折り返す
		ImGui::PushTextWrapPos();
		const float footerHeightToReserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
		if (ImGui::BeginChild("ScrollRegion##", ImVec2(0, -footerHeightToReserve), false, 0))
		{
			for (auto& text : ConsoleData::Instance().logs)
			{
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1));
				ImGui::TextUnformatted(text.c_str());
				ImGui::PopStyleColor();
			}
		}

		// ログを自動スクロール
		if ((ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
			ImGui::SetScrollHereY(1.0f);

		ImGui::EndChild();
	}
	ImGui::End();
}


