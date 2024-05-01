#include <sstream>
#include <dwmapi.h>
#include "Framework.h"
#include "Graphics/Graphics.h"
#include "ImGui/ImGuiManager.h"
#include "ImGui/ConsoleData.h"
#include "Icon/resource.h"
#include "Input/InputManager.h"
#include "Audio/AudioManager.h"
#include "Effekseer/EffectManager.h"
#include "Effekseer/Effect.h"
#include "3D/DebugPrimitive.h"
#include "3D/LineRenderer.h"
#include "../SceneManager.h"
#include "Timer.h"

// 初期化
bool Framework::Initialize(HINSTANCE hInstance)
{
	// --- ウィンドウ作成 ---
	CreateHSNWindow(hInstance);

	// --- Graphics初期化 ---
	Graphics::Instance().Initialize(hwnd, screenWidth, screenHeight);

	// --- ImGui初期化 ---
	ImGuiManager::Instance().Initialize(hwnd);

	// --- InputManager 初期化 ---
	InputManager::Instance().Initialize(hwnd);

	// --- AudioManager 初期化 ---
	AudioManager::Instance().Initialize();

	// --- EffectManager 初期化 ---
	EffectManager::Instance().Initialize();
	Effect::Instance().Initialize();

	// --- DebugPrimitive 初期化 ---
	DebugPrimitive::Instance().Initialize();

	// --- LineRenderer 初期化 ---
	LineRenderer::Instance().Initialize();

	// --- シーン初期化 ---
	SceneManager::Instance().ChangeScene(new SceneSTTitle); //TODO:SceneSTTitleから始まるように変更

	return true;
}

// 更新
void Framework::Update()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));	// メッセージ構造体を初期化

	// --- メインループ ---
	while (msg.message != WM_QUIT)
	{
		// --- メッセージの取得 ---
		if (PeekMessage(	// 戻り値：メッセージの取得結果（成功：true　失敗:false）
			&msg,			// メッセージ情報を受信するMSG構造体へのポインター
			NULL,			// メッセージを取得するウィンドウへのハンドル
			0,				// 検査するメッセージの範囲内の最初のメッセージの値
			0,				// 検査するメッセージの範囲内の最後のメッセージの値
			PM_REMOVE		// メッセージの処理方法（ここでは同じメッセージを２回読み込まないようにメッセージを削除している）
		))
		{
			TranslateMessage(&msg);		// 仮想キーメッセージを文字メッセージに変換
			DispatchMessage(&msg);		// メッセージをウィンドウプロシージャに送信（ウィンドウプロシージャを呼び出す処理みたいなイメージだと思う）
		}
		else
		// --- メイン処理 ---
		{
			// メッセージがない場合はこちらを処理する

			// --- タイマー処理 ---
			Timer::Instance().Tick();
			CalculateFrame();

			// --- inputManager処理 ---
			InputManager::Instance().Update();

			// --- シーン更新 ---
			ImGuiManager::Instance().Update();
			SceneManager::Instance().Update();
			EffectManager::Instance().Update();

			// --- シーン描画 ---
			std::lock_guard<std::mutex>	lock(Graphics::Instance().GetMutex());	// 排他制御

			Graphics::Instance().Begin();
			SceneManager::Instance().Render();
			ImGuiManager::Instance().Render();
			Graphics::Instance().End();
		}
	}
}

// 終了化
void Framework::Finalize()
{
	// --- EffectManager終了化 ---
	EffectManager::Instance().Finalize();

	// --- ImGui終了化 ---
	ImGuiManager::Instance().Finalize();

	// --- SceneManager終了化 ---
	SceneManager::Instance().Clear();
}

// ウィンドウ作成
void Framework::CreateHSNWindow(HINSTANCE hInstance)
{
	// --- アイコンの登録 ---
	HICON hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYICON));

	// --- ウィンドウクラスの登録 ---
	WNDCLASSEX wc;	//ウィンドウクラス情報の構造体
	wc.cbSize = sizeof(WNDCLASSEX);						// 構造体のサイズ
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;		// クラススタイルの組み合わせ(ウィンドウの幅や高さ等を変更した際に再描画する等？)
	wc.lpfnWndProc = WndProc;							// ウィンドウプロシージャのアドレス(メッセージが発生した際にこのプロシージャにメッセージが送られる)
	wc.cbClsExtra = 0;									// 補助メモリ
	wc.cbWndExtra = 0;									// 補助メモリ
	wc.hInstance = hInstance;							// このアプリのインスタンスハンドル
	wc.hIcon = hIcon;									// アプリのショートカット等で仕様されるアイコンを指定できる
	wc.hIconSm = hIcon;									// タイトルバーで使用されるアイコンの指定
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);			// ウィンドウ上のマウスカーソルを指定できる
	wc.hbrBackground = NULL;							// ウィンドウのクライアント領域の背景色
	wc.lpszMenuName = NULL;								// ウィンドウ上段に設定するメニューの名前（メニューがなければNULL）
	wc.lpszClassName = windowName;						// 登録時に使用するウィンドウクラスの名前

	RegisterClassExW(&wc);								// 情報をOSに登録								

	// --- ウィンドウサイズの調整 ---
	int centerScreenX = GetSystemMetrics(SM_CXSCREEN) / 2 - screenWidth / 2;
	int centerScreenY = GetSystemMetrics(SM_CYSCREEN) / 2 - screenHeight / 2;
	RECT wr;
	wr.left = centerScreenX;
	wr.top = centerScreenY;
	wr.right = wr.left + screenWidth;
	wr.bottom = wr.top + screenHeight;
	AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);

	// --- ウィンドウの作成 ---
	hwnd = CreateWindowEx(	// ウィンドウが正常に作成されなかった場合はNULLを返す
		0,					// 拡張ウィンドウスタイル
		windowName,			// ウィンドウクラス名
		windowName,			// ウィンドウタイトル名
		WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME, 	// ウィンドウスタイル
		wr.left,			// ウィンドウのX座標
		wr.top,				// ウィンドウのY座標
		wr.right - wr.left,	// ウィンドウの幅
		wr.bottom - wr.top,	// ウィンドウの高さ
		NULL,				// このウィンドウの親へのハンドル（最初のウィンドウのため親ウィンドウはない）
		NULL,				// メニューまたは子ウィンドウの識別子へのハンドル（メニューを使用したい場合はNULLに設定し、WindowClassExでmenuを使用することができる）
		hInstance,			// このウィンドウで使用するモジュールのインスタンスへのハンドル
		NULL				// ウィンドウを作成するパラメータ
	);

	// --- ウィンドウを表示 ---
	ShowWindow(hwnd, SW_SHOW);
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	// --- ダークモード有効時の設定 ---
	BOOL USE_DARK_MODE = true;
	BOOL SET_IMMERSIVE_DARK_MODE_SUCCESS = SUCCEEDED(DwmSetWindowAttribute(
		hwnd, DWMWINDOWATTRIBUTE::DWMWA_USE_IMMERSIVE_DARK_MODE,
		&USE_DARK_MODE, sizeof(USE_DARK_MODE)));

	// --- ウィンドウ境界線の色設定 ---
	COLORREF WINDOW_COLOR = 0xFFFFFF;
	DwmSetWindowAttribute(
		hwnd, DWMWINDOWATTRIBUTE::DWMWA_BORDER_COLOR,
		&WINDOW_COLOR, sizeof(WINDOW_COLOR));

	// --- キャプションのカラー変更 ---		
	COLORREF CAPTION_COLOR = 0x282828;
	DwmSetWindowAttribute(
		hwnd, DWMWINDOWATTRIBUTE::DWMWA_CAPTION_COLOR,
		&CAPTION_COLOR, sizeof(CAPTION_COLOR));

	// --- キャプションテキストのカラー変更 ---
	COLORREF CAPTION_TEXT_COLOR = 0xf1f1f1;
	DwmSetWindowAttribute(
		hwnd, DWMWINDOWATTRIBUTE::DWMWA_TEXT_COLOR,
		&CAPTION_TEXT_COLOR, sizeof(CAPTION_TEXT_COLOR));
}

// ウィンドウプロシージャ
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wparam, LPARAM lparam);

LRESULT Framework::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
	{
		return true;
	}

	switch (uMsg)
	{
	// ウィンドウを閉じた時
	case WM_CLOSE:
		PostMessage(hwnd, WM_DESTROY, 0, 0);
		break;
	// ウィンドウが破棄された時
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_SIZE:
	{
		if (Graphics::Instance().device != nullptr)
		{
			// ウィンドウの新しいサイズを取得
			int width = LOWORD(lParam);
			int height = HIWORD(lParam);
			Framework::Instance().windowWidth = width;
			Framework::Instance().windowHeight = height;
			// swapchain の再構築
			Graphics::Instance().CreateSwapchain(hwnd, width, height);
		}
	}
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			DestroyWindow(hwnd);
		}
		[[fallthrough]];
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		DirectX::Keyboard::ProcessMessage(uMsg, wParam, lParam);
		break;
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_XBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_XBUTTONUP:
	case WM_MOUSEWHEEL:
	case WM_MOUSEHOVER:
		DirectX::Mouse::ProcessMessage(uMsg, wParam, lParam);
		break;
	// 処理するメッセージがない場合
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

// FPS をタイトルバーにセット
void Framework::CalculateFrame()
{
	static UINT frame = 0;
	static float deltaTime = 0;

	frame++;

	if ((Timer::Instance().TimeStamp() - deltaTime) >= 1.0f)
	{
		float fps = static_cast<float>(frame);
		std::wostringstream outs;
		outs.precision(6);
		outs << windowName << 
			L" : FPS : " << fps << L" - " << 
			L"Frame Time : " << 1000.0f / fps << L" (ms) - " <<
			L"Delta Time : " << Timer::Instance().DeltaTime() << L" (s)";
		SetWindowTextW(hwnd, outs.str().c_str());

		frame = 0;
		deltaTime += 1.0f;

		//ConsoleData::Instance().logs.push_back("FPS : " + std::to_string(static_cast<int>(fps)));
	}
}
