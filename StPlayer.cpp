#include "StPlayer.h"
#include "Library/3D/DebugPrimitive.h"
#include "Library/3D/ResourceManager.h"
#include "Library/Input/InputManager.h"
#include "Library/ImGui/Include/imgui.h"
#include "Library/Timer.h"
#include "Library/3D/Camera.h"

#include "SpinningTopEnemyManager.h"
#include "SpinningTopPlayerManager.h"
#include "ObstacleManager.h"
#include "Collision.h"
#include "DataManager.h"

#include <typeinfo>
#include <algorithm>
#include "Library/Graphics/Graphics.h"
#include "DamageTextManager.h"
#include "ObsParts.h"
#include "Wave.h"
#include <DirectXMath.h>

using namespace DirectX;

#if _MSVC_LANG >= 202002L
static const char* u8cast(const char8_t* x) { return reinterpret_cast<const char*>(x); }
#else
static const char* u8cast(const char* x) { return x; }
#endif

StPlayer::StPlayer() {
	model = ResourceManager::Instance().LoadModelResource("Data/Fbx/StPlayer/Main/StPlayer.fbx");
	childModel = ResourceManager::Instance().LoadModelResource("Data/Fbx/StPlayerOption/koki_all.fbx");
	PlayerData dataArray[2];
	DataManager::Instance().LoadPlayerData(dataArray,ARRAYSIZE(dataArray));
	data = std::make_shared<PlayerData>(dataArray[0]);
	optionData = std::make_shared<PlayerData>(dataArray[1]);

	radius = data->radius;
	rotateSpeed = data->rotateInitialSpeed;

	isPlayer = true;

	// parryEffect
	parryEffect = std::make_unique<ParryEffect>(data->parryRadius);
	// domeEffect
	domeEffect = std::make_unique<DomeEffect>(data->parryRadius);
}

StPlayer::~StPlayer() {}

void StPlayer::Update() {

	if (isDead) return;

	Input();

	// �S�E�F�[�u�I���Ȃ���͂�ł�����
	if (Wave::Instance().state == 3)
	{
		DirectX::XMFLOAT2 zero = { 0,0 };
		inputMap["Move"] = zero;
		debugValue["Move"] = zero;
		inputMap["SubAttack"] = false;
		inputMap["Attack"] = false;
	}

	Camera::Instance().SetTarget(position);

	UpdateRotate();
	UpdateMove();
	UpdateObstacleCollision();
	UpdateAttack();
	UpdateDamaged();
	UpdateOption();

	// ���͍X�V����
	UpdateVelocity();
	// ���G���ԍX�V
	UpdateInvincibleTimer();

	//UpdateAnimation();

	// �I�u�W�F�N�g�s��X�V
	UpdateTransform();

	// parryEffect�X�V
	parryEffect->SetPosition({ position.x, 0.2f, position.z });
	parryEffect->Update();
	// domeEffect�X�V
	domeEffect->SetPosition({ position.x, 0.2f, position.z });
	domeEffect->Update();
}

void StPlayer::Render() {
	if (isDead) return;

	model->Render(transform, { 1,1,1,1 }, &keyFrame);
	parryEffect->Render();
	domeEffect->Render();
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

	if (ImGui::Begin(u8cast(u8"�v���C���["))) {
		if (ImGui::CollapsingHeader(u8cast(u8"�v���p�e�B"), ImGuiTreeNodeFlags_DefaultOpen)) {
			if (ImGui::Button(u8cast(u8"�ۑ�"))) {
				data->radius = radius;
				optionData->radius;
				PlayerData array[] = {*data.get(),*optionData.get()};
				DataManager::Instance().SavePlayerData(array,ARRAYSIZE(array));
			}

			if (ImGui::CollapsingHeader(u8cast(u8"�ړ�"), ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::DragFloat(u8cast(u8"�@����"), &data->mobility, 0.01f);
				ImGui::DragFloat(u8cast(u8"�����x"), &data->accel, 0.1f);
				if (data->accel < 0)data->accel = 0;
				ImGui::DragFloat(u8cast(u8"�����x"), &data->slow, 0.1f);
				ImGui::DragFloat(u8cast(u8"���@�̔��a"), &radius, 0.1f);
			}

			if (ImGui::CollapsingHeader(u8cast(u8"��]"), ImGuiTreeNodeFlags_DefaultOpen)) {
				XMFLOAT2 angleF2;
				XMStoreFloat2(&angleF2, XMVector2Transform(XMVectorSet(0, 1, 0, 0), XMMatrixRotationZ(XMConvertToRadians(angle.y))));
				ImguiVectorDirDraw(20, 10, 20, angleF2);
				ImGui::DragFloat(u8cast(u8"������]���x(�Q�[�W�U���ɂ��g��)"), &data->rotateInitialSpeed, 0.01f);
				ImGui::DragFloat(u8cast(u8"�ő��]���x"), &data->rotateMaxSpeed, 0.1f);
				ImGui::DragFloat(u8cast(u8"��]����(�f�o�b�O�p)"), &angle.y);
			}

			if (ImGui::CollapsingHeader(u8cast(u8"�p���B"), ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::DragFloat(u8cast(u8"�m�b�N�o�b�N"), &data->parryKnockback, 0.1f);
				ImGui::DragFloat(u8cast(u8"���a"), &data->parryRadius, 0.1f);
				ImGui::DragFloat(u8cast(u8"�ő唼�a"), &data->parryDamageMaxRadius, 0.1f);
				ImGui::DragFloat(u8cast(u8"�傫���Ȃ鑬�x(����)"), &data->parryDamageIncrementSpeed, 0.001f, 0, 1, "%.4f");
				ImGui::DragFloat(u8cast(u8"�N�[���_�E��"), &data->parryCooldown, 0.1f);

				ImGui::DragFloat(u8cast(u8"�Q�[�W���a"), &data->parryGaugeRadius, 0.1f);
				ImGui::DragFloat(u8cast(u8"�Q�[�W�ő唼�a"), &data->parryGaugeDamageMaxRadius, 0.1f);
				ImGui::DragFloat(u8cast(u8"�Q�[�W����"), &data->parryGaugeConsumed, 0.1f);
			}

			if (ImGui::CollapsingHeader(u8cast(u8"�_���[�W��"), ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::DragFloat(u8cast(u8"�m�b�N�o�b�N"), &data->damagedKnockback, 0.1f);
			}
		}

		if (ImGui::CollapsingHeader(u8cast(u8"�q�@"), ImGuiTreeNodeFlags_DefaultOpen)) {
			if (ImGui::Button(u8cast(u8"�ǉ�"))) AddOption();
			if (ImGui::Button(u8cast(u8"�폜"))) EraseOption();

			ImGui::DragFloat(u8cast(u8"����"), &data->optionRange, 0.1f);

			if (ImGui::CollapsingHeader(u8cast(u8"�v���p�e�B�q�@"), ImGuiTreeNodeFlags_DefaultOpen)) {
				if (ImGui::CollapsingHeader(u8cast(u8"�ړ��q�@"), ImGuiTreeNodeFlags_DefaultOpen)) {
					ImGui::DragFloat(u8cast(u8"�q�@�̔��a�q�@"), &radius, 0.1f);
				}

				if (ImGui::CollapsingHeader(u8cast(u8"��]�q�@"), ImGuiTreeNodeFlags_DefaultOpen)) {
					XMFLOAT2 angleF2;
					XMStoreFloat2(&angleF2, XMVector2Transform(XMVectorSet(0, 1, 0, 0), XMMatrixRotationZ(XMConvertToRadians(angle.y))));
					ImguiVectorDirDraw(20, 10, 20, angleF2);
					ImGui::DragFloat(u8cast(u8"������]���x�q�@"), &optionData->rotateInitialSpeed, 0.01f);
				}

				if (ImGui::CollapsingHeader(u8cast(u8"�p���B�q�@"), ImGuiTreeNodeFlags_DefaultOpen)) {
					ImGui::DragFloat(u8cast(u8"�m�b�N�o�b�N�q�@"), &optionData->parryKnockback, 0.1f);
					ImGui::DragFloat(u8cast(u8"���a�q�@"), &optionData->parryRadius, 0.1f);
					ImGui::DragFloat(u8cast(u8"�ő唼�a�q�@"), &optionData->parryDamageMaxRadius, 0.1f);
					ImGui::DragFloat(u8cast(u8"�N�[���_�E���q�@"), &optionData->parryCooldown, 0.1f);

					ImGui::DragFloat(u8cast(u8"�Q�[�W���a�q�@"), &optionData->parryGaugeRadius, 0.1f);
					ImGui::DragFloat(u8cast(u8"�Q�[�W�ő唼�a�q�@"), &optionData->parryGaugeDamageMaxRadius, 0.1f);
					ImGui::DragFloat(u8cast(u8"�Q�[�W����q�@"), &optionData->parryGaugeConsumed, 0.1f);
				}
			}
		}

		if (ImGui::CollapsingHeader(u8cast(u8"�f�o�b�N"), ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::DragFloat("speed", &rotateSpeed, .1f);
			if (ImGui::CollapsingHeader(u8cast(u8"����"), ImGuiTreeNodeFlags_DefaultOpen)) {
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

			if (ImGui::CollapsingHeader(u8cast(u8"�ړ�"), ImGuiTreeNodeFlags_DefaultOpen)) {
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
				ImGui::Text(u8cast(u8"����FPS:%f"), ave);
				ImGui::Text(u8cast(u8"�ŏ�FPS:%f"), *std::min_element(averageFPS.begin(), averageFPS.end()));
				ImGui::Text(u8cast(u8"�ő�FPS:%f"), *std::max_element(averageFPS.begin(), averageFPS.end()));

				static bool lowfps = false;
				static int loopcount = 10000000;
				ImGui::Checkbox(u8cast(u8"FPS��Ⴍ����"), &lowfps);
				ImGui::DragInt(u8cast(u8"���[�v��"), &loopcount, 1000000);
				if (loopcount < 1) loopcount = 1;
				if (lowfps) for (size_t i = 0; i < loopcount; i++);

				static bool drawPrimitive = false;
				ImGui::Checkbox(u8cast(u8"�f�o�b�N�p�����蔻���\��"), &drawPrimitive);
				if (drawPrimitive) RenderDebugPrimitive();

			}
		}
		ImGui::End();
	}


}

void StPlayer::UpdateMove() {
	float deltaTime = Timer::Instance().DeltaTime();
	XMFLOAT2 input = GetInputMap<XMFLOAT2>("Move");
	XMFLOAT2 moveDirection;
	XMVECTOR inputVec = XMLoadFloat2(&input);
	XMVECTOR direction = XMVectorSet(velocity.x, velocity.z, 0, 0);
	float speed = XMVectorGetX(XMVector2Length(direction));
	direction = XMVector2Normalize(direction);

	//�R���g���[���[�p
	if (XMVectorGetX(XMVector2Length(inputVec)) > 1) {
		inputVec = XMVector2Normalize(inputVec);
	}
	debugValue["inputVec"] = input;

	//���x�v�Z
	if (fabsf(input.x) > 0.00001f || fabsf(input.y) > 0.00001f) {
		float dot = XMVectorGetX(XMVector2Dot(direction, inputVec));
		if (speed == 0) dot = 1;
		speed += XMVectorGetX(XMVector2Length(inputVec) * dot) * data->accel * deltaTime;
		speed = max(0.f, speed);
	}
	else speed = max(0.0f, speed - (data->slow * deltaTime));

	speed -= speed * 0.998f * deltaTime;
	maxMoveSpeed = (data->accel + (data->accel * 0.002f));
	speed = min(speed, maxMoveSpeed);

	//�@�����̌v�Z
	if (speed < 0.0000001) direction = {};
	float lerp = min(max(0, (speed / maxMoveSpeed * data->mobility) + (1 - data->mobility)), 0.95f);
	direction = XMVectorLerp(direction, XMVectorLerp(inputVec, direction, lerp), 30 * deltaTime);
	direction = XMVector2Normalize(direction);
	XMStoreFloat2(&moveDirection, direction);

	//�f�o�b�O�p
	debugValue["speed"] = speed;
	debugValue["direction"] = moveDirection;
	debugValue["directionLength"] = XMVectorGetX(XMVector2Length(XMLoadFloat2(&moveDirection)));
	debugValue["dot"] = XMVectorGetX(XMVector2Dot(direction, inputVec));
	debugValue["t"] = lerp;

	velocity.x = moveDirection.x * speed;
	velocity.z = moveDirection.y * speed;

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
			XMStoreFloat3(&out1, XMVector3Normalize(XMLoadFloat3(&out1)) * data->damagedKnockback);

			velocity = out1;
			
			if (enemy->isDown)
			{
				enemy->createParts = true;
				
				int dmg = 300;

				totalHitScore += dmg;

				// �G���_�E�����Ȃ�G��j��
				enemy->ApplyDamage(dmg, 0);

				DamageText* damageText = new DamageText({ enemy->GetPosition().x, 1.0f, enemy->GetPosition().z }, std::to_string(dmg), {1,0,0,1});
				DamageTextManager::Instance().Register(damageText);

				Effect::Instance().Play(EffectType::HitStDownEnemy, GetPosition(), { 0,0,0 }, 1.0f);
			}
			else
			{
				// �_���[�W�\��
				if (invincibleTimer <= 0.0f)
				{
					//int damage = 1;
					//std::wstring damageString = std::to_wstring(damage);
					//const TCHAR* damageTChar = damageString.c_str();
					//DamageText* damageText = new DamageText({ GetPosition().x, 1.0f, GetPosition().z }, damageTChar, { 1,0,0,1 });
					//DamageText* damageText = new DamageText({ GetPosition().x, 1.0f, GetPosition().z }, "1", {1,0,0,1});
					//DamageTextManager::Instance().Register(damageText);

					Effect::Instance().Play(EffectType::HitStVsSt, GetPosition(), { 0,0,0 }, 1.0f);
				}

				// �G���_�E�����łȂ��Ȃ�v���C���[�Ƀ_���[�W
				ApplyDamage(1, invicibleTime);
			}
			

			break;
		}
	}

}

void StPlayer::UpdateObstacleCollision()
{
	ObstacleManager& obsM = ObstacleManager::Instance();

	int obsCount = obsM.GetObstacleCount();
	for (int i = 0; i < obsCount; i++)
	{
		Obstacle* obs = obsM.GetObstacle(i);

		if (!obs->isCollision) continue;

		// �Փˏ���
		DirectX::XMFLOAT3 velocityA;
		if (Collision::StaticRepulsionSphereVsSphere(
			GetPosition(),
			GetRadius(),
			obs->GetPosition(),
			obs->GetRadius(),
			velocityA,
			120
		))
		{
			// �����o����̈ʒu�ݒ�
			DirectX::XMFLOAT3 playerAVel = GetVelocity();
			DirectX::XMVECTOR V_A = DirectX::XMLoadFloat3(&playerAVel);

			DirectX::XMVECTOR AddA = DirectX::XMLoadFloat3(&velocityA);

			V_A = DirectX::XMVectorAdd(V_A, AddA);
			DirectX::XMStoreFloat3(&velocityA, V_A);

			SetVelocity(velocityA);

			if (obs->hitDamae > 0)
			{
				if (invincibleTimer <= 0.0f)
				{
					// �_���[�W�\��
					//std::wstring damageString = std::to_wstring(obs->hitDamae);
					//const TCHAR* damageTChar = damageString.c_str();
					//DamageText* damageText = new DamageText({ GetPosition().x, 1.0f, GetPosition().z }, damageTChar, { 1,0,0,1 });
					DamageText* damageText = new DamageText({ GetPosition().x, 1.0f, GetPosition().z }, "1", { 1,0,0,1 });
					DamageTextManager::Instance().Register(damageText);

					Effect::Instance().Play(EffectType::HitStVsSt, GetPosition(), { 0,0,0 }, 1.0f);
				}
				ApplyDamage(obs->hitDamae, invicibleTime);	
			}
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

	if (isParrySuccessed) rotateSpeed += ((rand() % 11) + 10);
	if (isParryGaugeSuccessed) rotateSpeed -= data->parryGaugeConsumed;
	rotateSpeed = min(rotateSpeed, data->rotateMaxSpeed);
	angle.y += 360 * rotateSpeed * Timer::Instance().DeltaTime();
	if (360 < angle.y || angle.y < -360)angle.y += angle.y > 0 ? -360 : 360;
}

void StPlayer::UpdateOption() {
	optionAngle += Timer::Instance().DeltaTime();

	for (int i = 0; i < option.size(); i++) {
		XMVECTOR playerPosVec = XMLoadFloat3(&position);
		XMFLOAT3 optionPos = option[i]->GetPosition();
		float angle = optionAngle + (XM_2PI / option.size() * i);

		XMStoreFloat3(&optionPos, (playerPosVec + XMVector3Transform(XMVectorSet(0, 0, 1, 0), XMMatrixRotationY(angle)) * data->optionRange));

		option[i]->SetPosition(optionPos);
	}
}

void StPlayer::AddOption() {
	getTotalOption++;
	StPlayerOption* o = new StPlayerOption(childModel,optionData);
	option.emplace_back(o);
	SpinningTopPlayerManager::Instance().Register(o);
}

void StPlayer::EraseOption() {
	if (option.empty())return;
	SpinningTopPlayerManager::Instance().Remove(option.back());
	option.pop_back();
}

void StPlayer::OnLanding() {}

void StPlayer::OnDamaged() {

}

void StPlayer::OnDead() {
	// �e�p�[�c�̐���
	ObsParts* top;
	ObsParts* middle;
	ObsParts* bottom;

	top = new ObsParts("Data/Fbx/StPlayer/Top/StPlayerTop.fbx", true);
	top->SetPosition(GetPosition());
	ObstacleManager::Instance().Register(top);

	middle = new ObsParts("Data/Fbx/StPlayer/Middle/StPlayerMiddle.fbx", true);
	middle->SetPosition(GetPosition());
	ObstacleManager::Instance().Register(middle);

	bottom = new ObsParts("Data/Fbx/StPlayer/Bottom/StPlayerBottom.fbx", true);
	bottom->SetPosition(GetPosition());
	bottom->SetAngle({ 90,0,0 });
	ObstacleManager::Instance().Register(bottom);

	// �q�@��S�č폜
	while (!option.empty())
	{
		EraseOption();
	}

	// ���S�t���O��ON
	isDead = true;
}
