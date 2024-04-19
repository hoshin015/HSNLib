#include "StPlayer.h"
#include "Library/3D/DebugPrimitive.h"
#include "Library/3D/ResourceManager.h"
#include "Library/Input/InputManager.h"
#include "Library/ImGui/Include/imgui.h"
#include "Library/Timer.h"

#include "SpinningTopEnemyManager.h"
#include "Collision.h"

using namespace DirectX;

StPlayer::StPlayer() {
	model = ResourceManager::Instance().LoadModelResource("Data/Fbx/SpinningTopTest/SpinningTopTest.fbx");

	//moveDirection = { 0,0 };
	//speed = 0;

	radius = 1;
	mobility = 1.f;
	rotateSpeed = 560;
	accel = 10;
	slow = 1;

	parryRadius = 1;
	parryKnockback = 30;

}

StPlayer::~StPlayer() {}

void StPlayer::Update() {
	Input();

	angle.y += rotateSpeed * Timer::Instance().DeltaTime();
	DebugPrimitive::Instance().AddSphere(position, parryRadius, { 1,1,255,255 });
	UpdateMove();
	UpdateAttack();

	// 速力更新処理
	UpdateVelocity();
	// 無敵時間更新
	UpdateInvincibleTimer();

	//UpdateAnimation();

	// オブジェクト行列更新
	UpdateTransform();
}

void StPlayer::Render() {
	model->Render(transform, { 0,1,0,1 }, &keyFrame);
}

void StPlayer::DrawDebugGui() {
	auto ImguiVectorDirDraw = [](float centerX, float centerY, float radius, XMFLOAT2 dir) {
		ImVec2 center = ImGui::GetCursorScreenPos();
		center.x += centerX;
		center.y += centerY;

		ImGui::GetWindowDrawList()->AddCircleFilled(center, radius, IM_COL32(128, 128, 128, 255), 32);
		ImVec2 arrowEnd = { center.x + dir.x * radius,center.y + dir.y * radius };
		ImGui::GetWindowDrawList()->AddLine(center, arrowEnd, IM_COL32(0, 255, 0, 255), 2.f);
	};

	if (ImGui::Begin("Player")) {
		if (ImGui::CollapsingHeader(u8"パラメータ", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::DragFloat("Mobility", &mobility, 0.01f);
			ImGui::DragFloat("Accel", &accel, 0.1f);
			if (accel < 0)accel = 0;
			ImGui::DragFloat("Slow", &slow, 0.1f);

			ImGui::DragFloat("ParryKnockback", &parryKnockback, 0.1f);
			ImGui::DragFloat("ParryRadius", &parryRadius, 0.1f);
		}

		if (ImGui::CollapsingHeader(u8"デバック", ImGuiTreeNodeFlags_DefaultOpen)) {
			if (ImGui::CollapsingHeader(u8"入力", ImGuiTreeNodeFlags_DefaultOpen)) {
				XMFLOAT2 outDebugFloat2;
				float outDebugFloat;
				int outDebugInt;

				outDebugFloat2 = GetInputMap<XMFLOAT2>("Move");
				outDebugFloat2.x *= -1;
				ImGui::Text("Move:%.3f,%.3f", outDebugFloat2.x, outDebugFloat2.y);
				ImguiVectorDirDraw(20, 10, 20, outDebugFloat2);

				outDebugInt = GetInputMap<bool>("Attack");
				ImGui::Text("Attack:%d", outDebugInt);

			}

			if (ImGui::CollapsingHeader(u8"移動", ImGuiTreeNodeFlags_DefaultOpen)) {
				XMFLOAT2 outDebugFloat2;
				float outDebugFloat;
				int outDebugInt;

				outDebugFloat2 = { GetDebugValue<float>("directionX"),GetDebugValue<float>("directionY") };
				outDebugFloat2.x *= -1;
				ImGui::Text("direction:%.3f,%.3f", outDebugFloat2.x, outDebugFloat2.y);
				ImguiVectorDirDraw(20, 10, 20, outDebugFloat2);

				outDebugFloat = GetDebugValue<float>("directionLength");
				ImGui::Text("directionLength:%.3f", outDebugFloat);

				outDebugFloat = GetDebugValue<float>("dot");
				ImGui::Text("dot:%.3f", outDebugFloat);

				outDebugFloat = GetDebugValue<float>("speed");
				ImGui::Text("speed:%.3f", outDebugFloat);

				outDebugFloat = GetDebugValue<float>("t");
				ImGui::Text("t:%.3f", outDebugFloat);

				static bool lowfps = false;
				static int loopcount = 10000000;
				ImGui::Checkbox("LowFPS", &lowfps);
				ImGui::DragInt("LoopCount", &loopcount,1000000);
				if (loopcount<1) loopcount = 1;
				if (lowfps) for (size_t i = 0; i < loopcount; i++);

			}
		}
		ImGui::End();
	}
}

void StPlayer::Input() {
	InputManager& im = InputManager::Instance();

	//Move
	XMFLOAT2 movefloat2;
	movefloat2.x = im.GetKeyPress(Keyboard::A) - im.GetKeyPress(Keyboard::D);
	movefloat2.y = im.GetKeyPress(Keyboard::S) - im.GetKeyPress(Keyboard::W);
	XMStoreFloat2(&movefloat2, XMVector2Normalize(XMLoadFloat2(&movefloat2)));

	if (im.IsGamePadConnected()) {
		movefloat2.x = -im.GetThumSticksLeftX();
		movefloat2.y = -im.GetThumSticksLeftY();
	}

	inputMap["Move"] = movefloat2;

	//Attack
	bool attack = im.GetKeyPressed(Keyboard::Space) || im.GetGamePadButtonPress(GAMEPADBUTTON_STATE::a);
	inputMap["Attack"] = attack;
}

void StPlayer::UpdateMove() {
	float deltaTime = Timer::Instance().DeltaTime();
	XMFLOAT2 input = GetInputMap<XMFLOAT2>("Move");
	XMFLOAT2 moveDirection;
	XMVECTOR inputVec = XMLoadFloat2(&input);
	XMVECTOR direction = XMVectorSet(velocity.x,velocity.z,0,0);
	float speed = XMVectorGetX(XMVector2Length(direction));
	direction = XMVector2Normalize(direction);

	//コントローラー用
	if (XMVectorGetX(XMVector2Length(inputVec)) > 1) {
		inputVec = XMVector2Normalize(inputVec);
	}

	//速度計算
	if (fabsf(input.x) > 0.00001f || fabsf(input.y) > 0.00001f) {
		float dot = XMVectorGetX(XMVector2Dot(direction, inputVec));
		if (speed == 0) dot = 1;
		speed += XMVectorGetX(XMVector2Length(inputVec) * dot) * accel * deltaTime;
		speed = max(0.f, speed);
	}
	else speed = max(0.0f, speed - (slow * deltaTime));

	speed -= speed * 0.998f * deltaTime;
	maxMoveSpeed = (accel + (accel * 0.002f));
	speed = min(speed, maxMoveSpeed);

	//機動性の計算
	if (speed < 0.0000001) direction = {};
	float lerp = min(max(0,(speed / maxMoveSpeed * mobility)+(1 - mobility)), 0.95f);
	direction = XMVectorLerp(direction, XMVectorLerp(inputVec, direction, lerp), 30 * deltaTime);
	direction = XMVector2Normalize(direction);
	XMStoreFloat2(&moveDirection, direction);

	//デバッグ用
	debugValue["speed"] = speed;
	debugValue["directionX"] = moveDirection.x;
	debugValue["directionY"] = moveDirection.y;
	debugValue["directionLength"] = XMVectorGetX(XMVector2Length(XMLoadFloat2(&moveDirection)));
	debugValue["dot"] = XMVectorGetX(XMVector2Dot(direction, inputVec));
	debugValue["t"] = lerp;

	velocity.x = moveDirection.x * speed;
	velocity.z = moveDirection.y * speed;

}

void StPlayer::UpdateAttack() {
	if (!GetInputMap<bool>("Attack")) return;

	SpinningTopEnemyManager& stEManager = SpinningTopEnemyManager::Instance();

	for (int i = 0; i < stEManager.GetEnemyCount(); i++) {
		SpinningTopEnemy* enemy = stEManager.GetEnemy(i);
		XMVECTOR pPosVec = XMLoadFloat3(&position);
		XMVECTOR ePosVec = XMLoadFloat3(&enemy->GetPosition());
		float eRadius = enemy->GetRadius();
		float distance = XMVectorGetX(XMVector3Length(ePosVec - pPosVec));

		if (eRadius + parryRadius > distance) {
			XMVECTOR knockbackVec = XMVector3Normalize(pPosVec - ePosVec);
			XMFLOAT3 result;

#if 0
			XMStoreFloat3(&result, (-knockbackVec) * parryKnockback);
			enemy->SetVelocity(result);

			XMStoreFloat3(&result, (knockbackVec) * parryKnockback);
			velocity = result;
#else
			XMFLOAT3 out1, out2;
			XMFLOAT3 eVel(enemy->GetVelocity());
			Collision::RepulsionSphereVsSphere(position, parryRadius, 1, enemy->GetPosition(), eRadius, 1, out1, out2);
			XMStoreFloat3(&out1, XMVector2Normalize(XMLoadFloat3(&out1) * XMVector3Length(XMLoadFloat3(&velocity))));
			XMStoreFloat3(&out2, XMVector2Normalize(XMLoadFloat3(&out2) * XMVector3Length(XMLoadFloat3(&eVel))));

			velocity = out1;
			enemy->SetVelocity(out2);
#endif
			DebugPrimitive::Instance().AddSphere(position, parryRadius, { 0,1,0,1 });
		}

	}
}

void StPlayer::OnLanding() {}

void StPlayer::OnDamaged() {}

void StPlayer::OnDead() {}
