#include "StPlayer.h"
#include "Library/3D/DebugPrimitive.h"
#include "Library/3D/ResourceManager.h"
#include "Library/Input/InputManager.h"
#include "Library/ImGui/Include/imgui.h"
#include "Library/Timer.h"
#include "Library/3D/Camera.h"

#include "SpinningTopEnemyManager.h"
#include "Obstacle.h"
#include "Collision.h"
#include "DataManager.h"

#include <typeinfo>
#include <algorithm>

using namespace DirectX;

#if _MSVC_LANG >= 202002L
static const char* u8cast(const char8_t* x) { return reinterpret_cast<const char*>(x); }
#else
static const char* u8cast(const char* x) { return x; }
#endif

StPlayer::StPlayer() {
	model = ResourceManager::Instance().LoadModelResource("Data/Fbx/StPlayer/StPlayer.fbx");
	childModel = model;
	DataManager::Instance().LoadPlayerData(&data);

	radius = data.radius;
	rotateSpeed = data.rotateInitialSpeed;

	isPlayer = true;
}

StPlayer::~StPlayer() {}

void StPlayer::Update() {
	Input();

	Camera::Instance().SetTarget(position);

	//UpdateEDistance(max(max(max(radius, data.parryRadius), parryDamageRadius), data.parryGaugeRadius));
	UpdateRotate();
	UpdateMove();
	UpdateAttack();
	UpdateDamaged();

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

	if (ImGui::Begin(u8cast(u8"プレイヤー"))) {
		if (ImGui::CollapsingHeader(u8cast(u8"プロパティ"), ImGuiTreeNodeFlags_DefaultOpen)) {
			if (ImGui::Button(u8cast(u8"保存"))) {
				data.radius = radius;
				DataManager::Instance().SavePlayerData(&data);
			}

			if (ImGui::CollapsingHeader(u8cast(u8"移動"), ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::DragFloat(u8cast(u8"機動性"), &data.mobility, 0.01f);
				ImGui::DragFloat(u8cast(u8"加速度"), &data.accel, 0.1f);
				if (data.accel < 0)data.accel = 0;
				ImGui::DragFloat(u8cast(u8"減速度"), &data.slow, 0.1f);
				ImGui::DragFloat(u8cast(u8"自機の半径"), &radius, 0.1f);
			}

			if (ImGui::CollapsingHeader(u8cast(u8"回転"), ImGuiTreeNodeFlags_DefaultOpen)) {
				XMFLOAT2 angleF2;
				XMStoreFloat2(&angleF2, XMVector2Transform(XMVectorSet(0, 1, 0, 0), XMMatrixRotationZ(XMConvertToRadians(angle.y))));
				ImguiVectorDirDraw(20, 10, 20, angleF2);
				ImGui::DragFloat(u8cast(u8"初期回転速度(ゲージ攻撃にも使う)"), &data.rotateInitialSpeed, 0.01f);
				ImGui::DragFloat(u8cast(u8"最大回転速度"), &data.rotateMaxSpeed, 0.1f);
				ImGui::DragFloat(u8cast(u8"回転方向(デバッグ用)"), &angle.y);
			}

			if (ImGui::CollapsingHeader(u8cast(u8"パリィ"), ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::DragFloat(u8cast(u8"ノックバック"), &data.parryKnockback, 0.1f);
				ImGui::DragFloat(u8cast(u8"半径"), &data.parryRadius, 0.1f);
				ImGui::DragFloat(u8cast(u8"最大半径"), &data.parryDamageMaxRadius, 0.1f);
				ImGui::DragFloat(u8cast(u8"大きくなる速度"), &data.parryDamageIncrementSpeed, 0.1f);
				ImGui::DragFloat(u8cast(u8"クールダウン"), &data.parryCooldown, 0.1f);

				ImGui::DragFloat(u8cast(u8"ゲージ半径"), &data.parryGaugeRadius, 0.1f);
				ImGui::DragFloat(u8cast(u8"ゲージ最大半径"), &data.parryGaugeDamageMaxRadius, 0.1f);
				ImGui::DragFloat(u8cast(u8"ゲージ消費"), &data.parryGaugeConsumed, 0.1f);
			}

			if (ImGui::CollapsingHeader(u8cast(u8"ダメージ受け"), ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::DragFloat(u8cast(u8"ノックバック"), &data.damagedKnockback, 0.1f);
			}
		}

		if (ImGui::CollapsingHeader(u8cast(u8"デバック"), ImGuiTreeNodeFlags_DefaultOpen)) {
			if (ImGui::CollapsingHeader(u8cast(u8"入力"), ImGuiTreeNodeFlags_DefaultOpen)) {
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

			if (ImGui::CollapsingHeader(u8cast(u8"移動"), ImGuiTreeNodeFlags_DefaultOpen)) {
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

				static std::vector<float> averageFPS(100);
				averageFPS.insert(averageFPS.begin(), 1 / Timer::Instance().DeltaTime());
				if (averageFPS.size() > 100) averageFPS.pop_back();
				float ave = 0;
				for (float& fps : averageFPS) ave += fps;
				ave = ave / averageFPS.size();
				ImGui::Text(u8cast(u8"平均FPS:%f"), ave);
				ImGui::Text(u8cast(u8"最小FPS:%f"), *std::min_element(averageFPS.begin(), averageFPS.end()));
				ImGui::Text(u8cast(u8"最大FPS:%f"), *std::max_element(averageFPS.begin(), averageFPS.end()));

				static bool lowfps = false;
				static int loopcount = 10000000;
				ImGui::Checkbox(u8cast(u8"FPSを低くする"), &lowfps);
				ImGui::DragInt(u8cast(u8"ループ回数"), &loopcount, 1000000);
				if (loopcount < 1) loopcount = 1;
				if (lowfps) for (size_t i = 0; i < loopcount; i++);

				static bool drawPrimitive = true;
				ImGui::Checkbox(u8cast(u8"デバック用当たり判定を表示"), &drawPrimitive);
				if (drawPrimitive) RenderDebugPrimitive();

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
	bool attack = im.GetKeyPressed(Keyboard::J) || im.GetGamePadButtonPressed(GAMEPADBUTTON_STATE::a);
	attack = attack && parryCooldownCount <= 0;
	inputMap["Attack"] = attack;

	bool subAttack = im.GetKeyPressed(Keyboard::K) || im.GetGamePadButtonPressed(GAMEPADBUTTON_STATE::b);
	subAttack = subAttack && (rotateSpeed > data.parryGaugeConsumed);
	inputMap["SubAttack"] = subAttack;
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
		speed += XMVectorGetX(XMVector2Length(inputVec) * dot) * data.accel * deltaTime;
		speed = max(0.f, speed);
	}
	else speed = max(0.0f, speed - (data.slow * deltaTime));

	speed -= speed * 0.998f * deltaTime;
	maxMoveSpeed = (data.accel + (data.accel * 0.002f));
	speed = min(speed, maxMoveSpeed);

	//機動性の計算
	if (speed < 0.0000001) direction = {};
	float lerp = min(max(0, (speed / maxMoveSpeed * data.mobility) + (1 - data.mobility)), 0.95f);
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
		parryDamageRadius += data.parryDamageIncrementSpeed * deltaTime;
		if (parryDamageRadius > data.parryRadius) {
			parryDamageRadius = 0;
			parry = false;
			hitEnemys.clear();
		}
	}

	if (parryGauge) {
		parryDamageRadius += data.parryDamageIncrementSpeed * deltaTime;
		if (parryDamageRadius > data.parryGaugeRadius) {
			parryDamageRadius = 0;
			parryGauge = false;
			hitEnemys.clear();
		}
	}

	if (parryCooldownCount > 0) {
		parryCooldownCount -= deltaTime;
	}
	debugValue["CooldownCount"] = parryCooldownCount;

	//パリィ成功,失敗&パルス内の判定 分けないとやばい気がする
	if (GetInputMap<bool>("Attack") || parry) {
		for (SpinningTopEnemy* enemy : nearEnemy) {
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

			if ((eRadius + data.parryRadius > distance) && !parry) {
				parry = true;
				break;
			}

			if (parry && eRadius + parryDamageRadius > distance) {
				hitEnemys.push_back(enemy);
#if 0
				XMVECTOR knockbackVec = XMVector3Normalize(pPosVec - ePosVec);
				XMFLOAT3 result;

				XMStoreFloat3(&result, (-knockbackVec) * parryKnockback);
				enemy->SetVelocity(result);

				XMStoreFloat3(&result, (knockbackVec)*parryKnockback);
				velocity = result;
#else
				XMFLOAT3 out1, out2;
				XMFLOAT3 eVel(enemy->GetVelocity());

				Collision::RepulsionSphereVsSphere(position, data.parryRadius, 1, enemy->GetPosition(), eRadius, 1, out1, out2);
				XMStoreFloat3(&out2, XMVector3Normalize(XMLoadFloat3(&out2)) * data.parryKnockback);

				enemy->SetVelocity(out2);
				enemy->ApplyDamage(1, 0);
#endif
			}

		}
	}

	if (GetInputMap<bool>("SubAttack") || parryGauge) {
		for (SpinningTopEnemy* enemy : nearEnemy) {
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

			if ((eRadius + data.parryGaugeRadius > distance) && !parryGauge) {
				parryGauge = true;
				break;
			}

			if (parryGauge && eRadius + parryDamageRadius > distance) {
				hitEnemys.push_back(enemy);

				XMFLOAT3 out1, out2;
				XMFLOAT3 eVel(enemy->GetVelocity());

				Collision::RepulsionSphereVsSphere(position, data.parryGaugeRadius, 1, enemy->GetPosition(), eRadius, 1, out1, out2);
				XMStoreFloat3(&out2, XMVector3Normalize(XMLoadFloat3(&out2)) * data.parryKnockback);

				enemy->SetVelocity(out2);
			}

		}
	}
	if (GetInputMap<bool>("Attack") && !parry) parryCooldownCount = data.parryCooldown;
}

void StPlayer::UpdateDamaged() {
	for (SpinningTopEnemy* enemy : nearEnemy) {
		XMVECTOR pPosVec = XMLoadFloat3(&position);
		XMVECTOR ePosVec = XMLoadFloat3(&enemy->GetPosition());
		float eRadius = enemy->GetRadius();
		float distance = XMVectorGetX(XMVector3Length(ePosVec - pPosVec));

		if (radius + eRadius > distance) {
			XMFLOAT3 out1, out2;

			Collision::RepulsionSphereVsSphere(position, radius, 1, enemy->GetPosition(), eRadius, 1, out1, out2);
			XMStoreFloat3(&out1, XMVector3Normalize(XMLoadFloat3(&out1)) * data.damagedKnockback);

			velocity = out1;
			ApplyDamage(1, 0);
			break;
		}
	}

}


void StPlayer::UpdateRotate() {
	static bool beforeState = false;
	static bool beforeStateGauge = false;
	bool isParrySuccessed = (beforeState ^ (parry)) && beforeState;
	bool isParryGaugeSuccessed = (beforeStateGauge ^ (parryGauge)) && beforeStateGauge;
	beforeState = parry;
	beforeStateGauge = parryGauge;

	if (isParrySuccessed) rotateSpeed++;
	if (isParryGaugeSuccessed) rotateSpeed -= data.parryGaugeConsumed;
	rotateSpeed = min(rotateSpeed, data.rotateMaxSpeed);
	angle.y += 360 * rotateSpeed * Timer::Instance().DeltaTime();
	if (36000 < angle.y || angle.y < -36000)angle.y = 0;
}

void StPlayer::RenderDebugPrimitive() {
	DebugPrimitive::Instance().AddSphere(position, radius, { 0,0,1,1 });
	DebugPrimitive::Instance().AddSphere(position, data.parryRadius, { 1,1,1,1 });
	if (rotateSpeed > data.parryGaugeConsumed)DebugPrimitive::Instance().AddSphere(position, data.parryGaugeRadius, { 1,0,1,1 });

	if(parry) DebugPrimitive::Instance().AddSphere(position, parryDamageRadius, { 0,1,0,1 });
	if(parryGauge) DebugPrimitive::Instance().AddSphere(position, parryDamageRadius, { 0,1,1,1 });
	if(parryCooldownCount > 0) DebugPrimitive::Instance().AddSphere(position, parryCooldownCount, { 1,0,0,1 });
}

void StPlayer::OnLanding() {}

void StPlayer::OnDamaged() {

}

void StPlayer::OnDead() {

}
