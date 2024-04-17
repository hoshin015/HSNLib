#include "StPlayer.h"
#include "Library/3D/DebugPrimitive.h"
#include "Library/3D/ResourceManager.h"
#include "Library/Input/InputManager.h"
#include "Library/ImGui/Include/imgui.h"
using namespace DirectX;

StPlayer::StPlayer() {
	model = ResourceManager::Instance().LoadModelResource("Data/Fbx/Jummo/Jummo.fbx");
}

StPlayer::~StPlayer() {}

void StPlayer::Update() {
	Input();

	angle.y+=2;

	// 速力更新処理
	UpdateVelocity();
	// 無敵時間更新
	UpdateInvincibleTimer();

	//UpdateAnimation();

	// オブジェクト行列更新
	UpdateTransform();
}

void StPlayer::Render() {
	model->Render(transform, { 0,1,1,1 }, &keyFrame);
}

void StPlayer::DrawDebugGui() {
	if (ImGui::Begin("Player")) {
		ImGui::Text("aaaa");
		if (ImGui::BeginListBox("Input")) {
			XMFLOAT2 outDebugFloat2;
			float outDebugFloat;
			int outDebugInt;

			outDebugFloat2 = std::get<XMFLOAT2>(inputAnalogMap["Move"]);
			ImGui::Text("Move:%.3f,%.3f", outDebugFloat2.x, outDebugFloat2.y);

			outDebugInt = inputDigitalMap["Attack"];
			ImGui::Text("Attack:%d", outDebugInt);

			ImGui::EndListBox();
		}
		ImGui::End();
	}
}

void StPlayer::Input() {
	InputManager& im = InputManager::Instance();

	XMFLOAT2 movefloat2;
	movefloat2.x = im.GetKeyPress(Keyboard::A) - im.GetKeyPress(Keyboard::D);
	movefloat2.y = im.GetKeyPress(Keyboard::S) - im.GetKeyPress(Keyboard::W);
	XMStoreFloat2(&movefloat2, XMVector2Normalize(XMLoadFloat2(&movefloat2)));

	if (im.IsGamePadConnected()) {
		movefloat2.x = im.GetThumSticksLeftX();
		movefloat2.y = im.GetThumSticksLeftY();
	}

	inputAnalogMap["Move"] = movefloat2;

	inputDigitalMap["Attack"] = im.GetKeyPressed(Keyboard::Space) | im.GetGamePadButtonPressed(GAMEPADBUTTON_STATE::a);
	Move(movefloat2.x, movefloat2.y, 10);
}

void StPlayer::OnLanding() {}

void StPlayer::OnDamaged() {}

void StPlayer::OnDead() {}
