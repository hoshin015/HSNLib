#include "StPlayer.h"
#include "Library/3D/DebugPrimitive.h"
#include "Library/3D/ResourceManager.h"
#include "Library/Input/InputManager.h"
#include "Library/ImGui/Include/imgui.h"
#include "Library/Timer.h"
using namespace DirectX;

StPlayer::StPlayer() {
	model = ResourceManager::Instance().LoadModelResource("Data/Fbx/Jummo/Jummo.fbx");

	maxSpeed = 10;
	rotateSpeed = 2;
	speed = 0;
	moveDirection = { 0,0 };
}

StPlayer::~StPlayer() {}

void StPlayer::Update() {
	Input();

	angle.y += 2;
	UpdateMove();

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
		ImGui::DragFloat("Speed", &maxSpeed, 0.1f);

		if (ImGui::CollapsingHeader(u8"インプット", ImGuiTreeNodeFlags_DefaultOpen)) {
			XMFLOAT2 outDebugFloat2;
			float outDebugFloat;
			int outDebugInt;

			outDebugFloat2 = GetInputMap<XMFLOAT2>("Move");
			ImGui::Text("Move:%.3f,%.3f", outDebugFloat2.x, outDebugFloat2.y);

			outDebugInt = GetInputMap<bool>("Attack");
			ImGui::Text("Attack:%d", outDebugInt);

		}
		if (ImGui::CollapsingHeader(u8"デバック", ImGuiTreeNodeFlags_DefaultOpen)) {
			XMFLOAT2 outDebugFloat2;
			float outDebugFloat;
			int outDebugInt;

			outDebugFloat2 = { GetDebugValue<float>("x"),GetDebugValue<float>("y") };
			ImGui::Text("debug:%.3f,%.3f", outDebugFloat2.x, outDebugFloat2.y);

			//outDebugFloat = GetDebugValue<float>("t");
			//ImGui::Text("t:%.3f", outDebugFloat);

			outDebugFloat = GetDebugValue<float>("direction");
			ImGui::Text("direction:%.3f", outDebugFloat);

			ImGui::Text("speed:%.3f", speed);
			ImGui::Text("frame:%.3f", 0.99999999f * Timer::Instance().DeltaTime());

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

	inputMap["Move"] = movefloat2;

	bool attack = im.GetKeyPress(Keyboard::Space) || im.GetGamePadButtonPress(GAMEPADBUTTON_STATE::a);
	inputMap["Attack"] = attack;
}

void StPlayer::UpdateMove() {
	XMFLOAT2 input = GetInputMap<XMFLOAT2>("Move");
	XMVECTOR inputVec = XMLoadFloat2(&input);
	XMVECTOR direction = XMLoadFloat2(&moveDirection);

	speed -= speed * 0.98f * Timer::Instance().DeltaTime();
	//inputVec *= speed;
	if (fabsf(input.x) > 0.00001f || fabsf(input.y) > 0.00001f) {
		speed += XMVectorGetX(XMVector2Length(inputVec)) * 10 * Timer::Instance().DeltaTime();
	}


	direction = XMVectorLerp(XMVector2Normalize(direction), inputVec,0.01f);

	XMStoreFloat2(&moveDirection, direction * speed);

	debugValue["x"] = moveDirection.x;
	debugValue["y"] = moveDirection.y;
	debugValue["direction"] = XMVectorGetX(XMVector2Length(XMLoadFloat2(&moveDirection)));

	Move(moveDirection.x, moveDirection.y, maxSpeed);
}

void StPlayer::OnLanding() {}

void StPlayer::OnDamaged() {}

void StPlayer::OnDead() {}
