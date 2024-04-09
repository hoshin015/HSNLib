#include "InputManager.h"

// 初期化処理
void InputManager::Initialize(HWND hwnd)
{
	// --- キーボード ---
	keyboard = std::make_unique<DirectX::Keyboard>();

	// --- マウス ---
	mouse = std::make_unique<DirectX::Mouse>();
	mouseButtonStates[static_cast<size_t>(MOUSEBUTTON_STATE::leftButton)]		= &mouseTracker.leftButton;
	mouseButtonStates[static_cast<size_t>(MOUSEBUTTON_STATE::middleButton)]		= &mouseTracker.middleButton;
	mouseButtonStates[static_cast<size_t>(MOUSEBUTTON_STATE::rightButton)]		= &mouseTracker.rightButton;
	mouseButtonStates[static_cast<size_t>(MOUSEBUTTON_STATE::xButton1)]			= &mouseTracker.xButton1;
	mouseButtonStates[static_cast<size_t>(MOUSEBUTTON_STATE::xButton2)]			= &mouseTracker.xButton2;
	SetMousePositionMode(DirectX::Mouse::MODE_ABSOLUTE);
	mouse->SetWindow(hwnd);

	// --- ゲームパッド ---
	gamepad = std::make_unique<DirectX::GamePad>();
	gamepadButtonStates[static_cast<size_t>(GAMEPADBUTTON_STATE::a)]				= &gamepadTracker.a;
	gamepadButtonStates[static_cast<size_t>(GAMEPADBUTTON_STATE::b)]				= &gamepadTracker.b;
	gamepadButtonStates[static_cast<size_t>(GAMEPADBUTTON_STATE::x)]				= &gamepadTracker.x;
	gamepadButtonStates[static_cast<size_t>(GAMEPADBUTTON_STATE::y)]				= &gamepadTracker.y;
	gamepadButtonStates[static_cast<size_t>(GAMEPADBUTTON_STATE::leftStick)]		= &gamepadTracker.leftStick;
	gamepadButtonStates[static_cast<size_t>(GAMEPADBUTTON_STATE::rightStick)]		= &gamepadTracker.rightStick;
	gamepadButtonStates[static_cast<size_t>(GAMEPADBUTTON_STATE::leftShoulder)]		= &gamepadTracker.leftShoulder;
	gamepadButtonStates[static_cast<size_t>(GAMEPADBUTTON_STATE::rightShoulder)]	= &gamepadTracker.rightShoulder;
	gamepadButtonStates[static_cast<size_t>(GAMEPADBUTTON_STATE::back)]				= &gamepadTracker.back;
	gamepadButtonStates[static_cast<size_t>(GAMEPADBUTTON_STATE::view)]				= &gamepadTracker.view;
	gamepadButtonStates[static_cast<size_t>(GAMEPADBUTTON_STATE::start)]			= &gamepadTracker.start;
	gamepadButtonStates[static_cast<size_t>(GAMEPADBUTTON_STATE::menu)]				= &gamepadTracker.menu;
	gamepadButtonStates[static_cast<size_t>(GAMEPADBUTTON_STATE::dpadUp)]			= &gamepadTracker.dpadUp;
	gamepadButtonStates[static_cast<size_t>(GAMEPADBUTTON_STATE::dpadDown)]			= &gamepadTracker.dpadDown;
	gamepadButtonStates[static_cast<size_t>(GAMEPADBUTTON_STATE::dpadLeft)]			= &gamepadTracker.dpadLeft;
	gamepadButtonStates[static_cast<size_t>(GAMEPADBUTTON_STATE::dpadRight)]		= &gamepadTracker.dpadRight;
}

// 更新処理
void InputManager::Update()
{
	// --- キーボード ---
	// キーボードの状態取得
	keyState = keyboard->GetState();
	// キーボード状態トラッカー更新
	keyTracker.Update(keyState);

	// --- マウス ---
	// マウスの状態取得
	mouseState = mouse->GetState();
	// マウス状態トラッカー更新
	mouseTracker.Update(mouseState);
	
	// --- ゲームパッド ---
	// ゲームパッドの状態取得
	gamepadState = gamepad->GetState(0);
	// ゲームパッド状態トラッカー更新
	gamepadTracker.Update(gamepadState);
}

// --- キーボード ---
bool InputManager::GetKeyPress(DirectX::Keyboard::Keys key)
{
	return keyState.IsKeyDown(key);
}

bool InputManager::GetKeyPressed(DirectX::Keyboard::Keys key)
{
	return keyTracker.IsKeyPressed(key);
}

bool InputManager::GetKeyReleased(DirectX::Keyboard::Keys key)
{
	return keyTracker.IsKeyReleased(key);
}



// ---	マウス ---
void InputManager::ResetScrollWheelValue()
{
	mouse->ResetScrollWheelValue();
}

bool InputManager::GetMousePress(MOUSEBUTTON_STATE button)
{
	return (*mouseButtonStates[static_cast<size_t>(button)] == DirectX::Mouse::ButtonStateTracker::HELD);
}

bool InputManager::GetMousePressed(MOUSEBUTTON_STATE button)
{
	return (*mouseButtonStates[static_cast<size_t>(button)] == DirectX::Mouse::ButtonStateTracker::PRESSED);
}

bool InputManager::GetMouseReleased(MOUSEBUTTON_STATE button)
{
	return (*mouseButtonStates[static_cast<size_t>(button)] == DirectX::Mouse::ButtonStateTracker::RELEASED);
}

// マウスカーソルの表示・非表示設定
void InputManager::SetMouseVisible(bool visible)
{
	mouse->SetVisible(visible);
}

// マウスの座標を絶対位置か相対位置か設定
void InputManager::SetMousePositionMode(DirectX::Mouse::Mode mode)
{
	mouse->SetMode(mode);
}

// ---	ゲームパッド ---
bool InputManager::IsGamePadConnected()
{
	return gamepadState.IsConnected();
}

bool InputManager::GetGamePadButtonPress(GAMEPADBUTTON_STATE button)
{
	return (*gamepadButtonStates[static_cast<size_t>(button)] == DirectX::GamePad::ButtonStateTracker::HELD);
}

bool InputManager::GetGamePadButtonPressed(GAMEPADBUTTON_STATE button)
{
	return (*gamepadButtonStates[static_cast<size_t>(button)] == DirectX::GamePad::ButtonStateTracker::PRESSED);
}

bool InputManager::GetGamePadButtonReleased(GAMEPADBUTTON_STATE button)
{
	return (*gamepadButtonStates[static_cast<size_t>(button)] == DirectX::GamePad::ButtonStateTracker::RELEASED);

}

void InputManager::SetVibration(int index, float leftPower, float rightPower)
{
	gamepad->SetVibration(index, leftPower, rightPower);
}

