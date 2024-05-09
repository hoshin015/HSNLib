#pragma once
#include <wtypes.h>
#include <string>
#include <vector>

//--------------------------------------------------------------
//  Framework
//--------------------------------------------------------------
class Framework
{
private:
	Framework() {}
	~Framework() {}

public:
	static Framework& Instance()
	{
		static Framework instance;
		return instance;
	}

public:
	int windowWidth = 1280;	//1280
	int windowHeight = 720;	//720

private:
	HWND hwnd;

	const wchar_t* windowName = L"Shot Away";
	const int screenWidth = 1280;
	const int screenHeight = 720;

	BOOL isFullScreen = FALSE;
public:
	// 初期化
	bool Initialize(HINSTANCE hInstance);
	// 更新
	void Update();
	// 終了化
	void Finalize();

private:
	// ウィンドウ作成
	void CreateHSNWindow(HINSTANCE hInstance);
	// ウィンドウプロシージャ
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	// FPS をタイトルバーにセット
	void CalculateFrame();
};