
#include "../Graphics/Graphics.h"
#include "../Timer.h"
#include "ImGuiManager.h"
#include "ConsoleData.h"


// ������
void ImGuiManager::Initialize(HWND hwnd)
{
	// ImGui�̏���
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	// Docking�̗L����
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	// ��ʊO�Ή�
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	// �E�B���h�E�̐[�x�ύX�Ή�
	io.ConfigViewportsNoDefaultParent = true;


	// Font
	io.Fonts->AddFontFromFileTTF("Data/Font/meiryob.ttc", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());

	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(Graphics::Instance().device.Get(), Graphics::Instance().deviceContext.Get());
	
	// �X�^�C���̐ݒ�
	ImGui::StyleColorsDark();

}

// �I����
void ImGuiManager::Finalize()
{
	// --- ImGui�̏I�� ---
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

// �X�V
void ImGuiManager::Update()
{
	// --- ImGui�̊J�n ---
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// --- DockSpace ���� ---
	DockSpace();
}

// �`��
void ImGuiManager::Render()
{
	// --- Console�`�� ---
	//Console();

	// --- ImGui�`�� ---
	ImGui::Render();	// �`��f�[�^�̑g�ݗ���
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}



// DockSpace����
void ImGuiManager::DockSpace()
{
	// https://gist.github.com/moebiussurfing/8dbc7fef5964adcd29428943b78e45d2

	// --- window�t���O�ݒ� ---
	static ImGuiWindowFlags windowFlags = 
		//ImGuiWindowFlags_MenuBar |		// �����L���ɂ���ƃ��j���[�o�[��ǉ��ł���
		ImGuiDockNodeFlags_PassthruCentralNode |
		ImGuiWindowFlags_NoDocking |
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse  | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;


	// --- dockNode�t���O�ݒ� ---
	const ImGuiDockNodeFlags dockFlags = ImGuiDockNodeFlags_PassthruCentralNode;

	// --- ImGui�̕\���ݒ� ---
	ImGuiViewport* viewport = ImGui::GetMainViewport();	// ���C���r���[�̒l���擾
	ImGui::SetNextWindowPos(viewport->WorkPos);			// �|�W�V�����̐ݒ�
	ImGui::SetNextWindowSize(viewport->Size, ImGuiCond_Always);		// �T�C�Y�̐ݒ�
	ImGui::SetNextWindowViewport(viewport->ID);			// ID�̐ݒ�
	ImGui::SetNextWindowBgAlpha(0.0f);					// �w�i�A���t�@�̐ݒ�

	// --- DockSpace�̎��̓X�^�C���̐ݒ�? ---
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	// --- DockSpace�̍쐬 ---
	ImGui::Begin("HSNLib", NULL, windowFlags);
	ImGui::PopStyleVar(3);	// ���̓X�^�C���̓K�p?

	ImGuiID dockSpaceID = ImGui::GetID("MyDockSpace");
	ImGui::DockSpace(dockSpaceID, ImVec2(0.f, 0.f), dockFlags);

	ImGui::End();
}

// Console����
void ImGuiManager::Console()
{
	ImGui::Begin("Console");
	{
		if (ImGui::Button("Clear", ImVec2(60, 20)))
		{
			ConsoleData::Instance().logs.clear();
		}

		// �e�L�X�g��܂�Ԃ�
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

		// ���O�������X�N���[��
		if ((ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
			ImGui::SetScrollHereY(1.0f);

		ImGui::EndChild();
	}
	ImGui::End();
}


