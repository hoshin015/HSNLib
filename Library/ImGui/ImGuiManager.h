#pragma once
#include <wtypes.h>
#include "./Include/imgui.h"
#include "./Include/imgui_impl_win32.h"
#include "./Include/imgui_impl_dx11.h"

#include <vector>
#include <string>

//--------------------------------------------------------------
//  Graphics
//--------------------------------------------------------------
class ImGuiManager
{
private:
	ImGuiManager() {}
	~ImGuiManager() {}

public:
	static ImGuiManager& Instance()
	{
		static ImGuiManager instance;
		return instance;
	}

	// ‰Šú‰»
	void Initialize(HWND hwnd);
	// I—¹‰»
	void Finalize();
	// XV
	void Update();
	// •`‰æ
	void Render();

	// DockSpaceˆ—
	void DockSpace();
	// Consoleˆ—
	void Console();
};