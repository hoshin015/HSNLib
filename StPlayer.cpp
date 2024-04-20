#include "StPlayer.h"
#include "Library/3D/DebugPrimitive.h"
#include "Library/3D/ResourceManager.h"
#include "Library/Input/InputManager.h"
#include "Library/ImGui/Include/imgui.h"
#include "Library/Timer.h"
#include "Library/3D/Camera.h"

#include "SpinningTopEnemyManager.h"
#include "Collision.h"

#include <typeinfo>

using namespace DirectX;

StPlayer::StPlayer() {
	model = ResourceManager::Instance().LoadModelResource("Data/Fbx/SpinningTopTest/SpinningTopTest.fbx");

	//moveDirection = { 0,0 };
	//speed = 0;

	radius = 1;
	mobility = 1.f;
	rotateIncrementSpeed = 2;
	accel = 10;
	slow = 1;

	parryRadius = 1;
	parryKnockback = 30;
	parryDamageMaxRadius = 10;
	parryDamageIncrementSpeed = 1;
	parryCooldown = 1;

}

StPlayer::~StPlayer() {}

void StPlayer::Update() {
	Input();

	Camera::Instance().SetTarget(position);

	angle.y += 360 * rotateIncrementSpeed * Timer::Instance().DeltaTime();
	if (360 < angle.y || angle.y < -360)angle.y = 0;
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
		if (ImGui::CollapsingHeader(u8"プロパティ", ImGuiTreeNodeFlags_DefaultOpen)) {
			if (ImGui::CollapsingHeader(u8"移動", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::DragFloat("Mobility", &mobility, 0.01f);
				ImGui::DragFloat("Accel", &accel, 0.1f);
				if (accel < 0)accel = 0;
				ImGui::DragFloat("Slow", &slow, 0.1f);
			}

			if (ImGui::CollapsingHeader(u8"回転", ImGuiTreeNodeFlags_DefaultOpen)) {
				XMFLOAT2 angleF2;
				XMStoreFloat2(&angleF2, XMVector2Transform(XMVectorSet(0, 1, 0, 0), XMMatrixRotationZ(XMConvertToRadians(angle.y))));
				ImguiVectorDirDraw(20, 10, 20, angleF2);
				ImGui::DragFloat("RotateIncrementSpeed", &rotateIncrementSpeed, 0.01f);
				ImGui::DragFloat("RotateIncrement", &rotateIncrement, 0.1f);
				ImGui::DragFloat("angle.y", &angle.y);
			}

			if (ImGui::CollapsingHeader(u8"パリィ", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::DragFloat("Knockback", &parryKnockback, 0.1f);
				ImGui::DragFloat("Radius", &parryRadius, 0.1f);
				ImGui::DragFloat("DamageMaxRadius", &parryDamageMaxRadius, 0.1f);
				ImGui::DragFloat("DamageSpeed", &parryDamageIncrementSpeed, 0.1f);
				ImGui::DragFloat("CoolDown", &parryCooldown, 0.1f);
			}
		}

		if (ImGui::CollapsingHeader(u8"デバック", ImGuiTreeNodeFlags_DefaultOpen)) {
			if (ImGui::CollapsingHeader(u8"入力", ImGuiTreeNodeFlags_DefaultOpen)) {
				XMFLOAT2 outDebugFloat2;
				float outDebugFloat;
				int outDebugInt;

				outDebugFloat2 = GetInputMap<XMFLOAT2>("Move");
				outDebugFloat2.x *= -1;
				ImguiVectorDirDraw(20, 10, 20, outDebugFloat2);

				for (int i = 0; i < inputMap.size(); i++) {
					auto it = inputMap.begin();
					std::advance(it, i);

					if (std::holds_alternative<XMFLOAT2>((it->second))) {
						outDebugFloat2 = std::get<XMFLOAT2>(it->second);
						ImGui::Text((it->first + ":%.3f,%.3f").c_str(), outDebugFloat2.x, outDebugFloat2.y);
					}

					if (std::holds_alternative<float>((it->second))) {
						outDebugFloat = std::get<float>(it->second);
						ImGui::Text((it->first + ":%.3f").c_str(), outDebugFloat);
					}

					if (std::holds_alternative<int>((it->second))) {
						outDebugInt = std::get<int>(it->second);
						ImGui::Text((it->first + ":%d").c_str(), outDebugInt);
					}

					if (std::holds_alternative<bool>((it->second))) {
						outDebugInt = std::get<bool>(it->second);
						ImGui::Text((it->first + ":%d").c_str(), outDebugInt);
					}
				}

			}

			if (ImGui::CollapsingHeader(u8"移動", ImGuiTreeNodeFlags_DefaultOpen)) {
				XMFLOAT2 outDebugFloat2;
				float outDebugFloat;
				int outDebugInt;

				outDebugFloat2 = GetDebugValue<XMFLOAT2>("direction");
				outDebugFloat2.x *= -1;
				ImguiVectorDirDraw(20, 10, 20, outDebugFloat2);

				for (int i = 0; i < debugValue.size(); i++) {
					auto it = debugValue.begin();
					std::advance(it, i);

					if (std::holds_alternative<XMFLOAT2>((it->second))) {
						outDebugFloat2 = std::get<XMFLOAT2>(it->second);
						ImGui::Text((it->first + ":%.3f,%.3f").c_str(), outDebugFloat2.x, outDebugFloat2.y);
					}

					if (std::holds_alternative<float>((it->second))) {
						outDebugFloat = std::get<float>(it->second);
						ImGui::Text((it->first + ":%.3f").c_str(), outDebugFloat);
					}

					if (std::holds_alternative<int>((it->second))) {
						outDebugInt = std::get<int>(it->second);
						ImGui::Text((it->first + ":%d").c_str(), outDebugInt);
					}

					if (std::holds_alternative<bool>((it->second))) {
						outDebugInt = std::get<bool>(it->second);
						ImGui::Text((it->first + ":%d").c_str(), outDebugInt);
					}
				}

				static bool lowfps = false;
				static int loopcount = 10000000;
				ImGui::Checkbox("LowFPS", &lowfps);
				ImGui::DragInt("LoopCount", &loopcount, 1000000);
				if (loopcount < 1) loopcount = 1;
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
	attack = attack && parryCooldownCount <= 0;
	inputMap["Attack"] = attack;
}

void StPlayer::UpdateMove() {
	float deltaTime = Timer::Instance().DeltaTime();
	XMFLOAT2 input = GetInputMap<XMFLOAT2>("Move");
	XMFLOAT2 moveDirection;
	XMVECTOR inputVec = XMLoadFloat2(&input);
	XMVECTOR direction = XMVectorSet(velocity.x, velocity.z, 0, 0);
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
	float lerp = min(max(0, (speed / maxMoveSpeed * mobility) + (1 - mobility)), 0.95f);
	direction = XMVectorLerp(direction, XMVectorLerp(inputVec, direction, lerp), 30 * deltaTime);
	direction = XMVector2Normalize(direction);
	XMStoreFloat2(&moveDirection, direction);

	//デバッグ用
	debugValue["speed"] = speed;
	debugValue["direction"] = moveDirection;
	debugValue["directionLength"] = XMVectorGetX(XMVector2Length(XMLoadFloat2(&moveDirection)));
	debugValue["dot"] = XMVectorGetX(XMVector2Dot(direction, inputVec));
	debugValue["t"] = lerp;

	velocity.x = moveDirection.x * speed;
	velocity.z = moveDirection.y * speed;

}

void StPlayer::UpdateAttack() {
	//パルス状のUpdate
	static std::vector<SpinningTopEnemy*> hitEnemys;
	float deltaTime = Timer::Instance().DeltaTime();
	if (parry) {
		parryDamageRadius += parryDamageIncrementSpeed * deltaTime;
		DebugPrimitive::Instance().AddSphere(position, parryDamageRadius, { 0,1,0,1 });
		if (parryDamageRadius > parryDamageMaxRadius) {
			parryDamageRadius = 0;
			parry = false;
			hitEnemys.clear();
		}
	}

	if (parryCooldownCount > 0) {
		parryCooldownCount -= deltaTime;
		DebugPrimitive::Instance().AddSphere(position, 1, { 1,0,0,1 });
	}
	debugValue["CooldownCount"] = parryCooldownCount;

	//パリィ成功,失敗&パルス内の判定 分けないとやばい気がする
	if (GetInputMap<bool>("Attack") || parry) {
		SpinningTopEnemyManager& stEManager = SpinningTopEnemyManager::Instance();
		for (int i = 0; i < stEManager.GetEnemyCount(); i++) {
			SpinningTopEnemy* enemy = stEManager.GetEnemy(i);

			if (hitEnemys.size() > 0) {
				bool hit = false;
				for (SpinningTopEnemy* hitEnemy : hitEnemys) {
					if (hitEnemy == enemy) {
						hit = true;
						break;
					}
				}
				if (hit) continue;
			}

			XMVECTOR pPosVec = XMLoadFloat3(&position);
			XMVECTOR ePosVec = XMLoadFloat3(&enemy->GetPosition());
			float eRadius = enemy->GetRadius();
			float distance = XMVectorGetX(XMVector3Length(ePosVec - pPosVec));

			if ((eRadius + parryRadius > distance) && !parry) {
				rotateIncrementSpeed++;
				parry = true;
				break;
			}

			if (parry && eRadius + parryDamageRadius > distance) {
#if 0
				XMVECTOR knockbackVec = XMVector3Normalize(pPosVec - ePosVec);
				XMFLOAT3 result;

				XMStoreFloat3(&result, (-knockbackVec) * parryKnockback);
				enemy->SetVelocity(result);

				XMStoreFloat3(&result, (knockbackVec)*parryKnockback);
				velocity = result;
#else
				hitEnemys.push_back(enemy);
				XMFLOAT3 out1, out2;
				XMFLOAT3 eVel(enemy->GetVelocity());

				Collision::RepulsionSphereVsSphere(position, parryRadius, 1, enemy->GetPosition(), eRadius, 1, out1, out2);
				XMStoreFloat3(&out1, XMVector2Normalize(XMLoadFloat3(&out1)) * parryKnockback);
				XMStoreFloat3(&out2, XMVector2Normalize(XMLoadFloat3(&out2)) * parryKnockback);

				//velocity = out1;
				enemy->SetVelocity(out2);
#endif
			}

		}
	}
	if(GetInputMap<bool>("Attack")&&!parry) parryCooldownCount = parryCooldown;
}

void StPlayer::OnLanding() {}

void StPlayer::OnDamaged() {}

void StPlayer::OnDead() {}
