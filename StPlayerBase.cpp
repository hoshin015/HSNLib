#include "StPlayerBase.h"
#include "Library/Input/InputManager.h"
#include "Library/Timer.h"
#include "Library/3D/DebugPrimitive.h"

#include "SpinningTopPlayerManager.h"
using namespace DirectX;

//TODO::一番大きい判定の中にいるEnemyを取得する もっといい方法あるかもしれない
void StPlayerBase::UpdateEDistance(const std::vector<StPlayerBase*>& players) {
	nearEnemy.clear();
	SpinningTopEnemyManager& stEManager = SpinningTopEnemyManager::Instance();
	for (int i = 0; i < stEManager.GetEnemyCount(); i++) {
		SpinningTopEnemy* enemy = stEManager.GetEnemy(i);
		if (enemy->GetHealth() <= 0) continue;
		XMVECTOR ePosVec = XMLoadFloat3(&enemy->GetPosition());
		float eRadius = enemy->GetRadius();

		for (auto& player : players) {
			float distance = XMVectorGetX(XMVector3Length(ePosVec - XMLoadFloat3(&player->position)));
			float highestRadius = max(max(max(player->radius, player->data->parryRadius), player->parryDamageRadius), player->data->parryGaugeRadius);
			if (highestRadius + eRadius > distance) {
				nearEnemy.push_back(enemy);
				break;
			}
		}
	}
}

void StPlayerBase::Input() {
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
	subAttack = subAttack && (rotateSpeed > data->parryGaugeConsumed);
	inputMap["SubAttack"] = subAttack;
}

void StPlayerBase::UpdateAttack() {
	//パルス状のUpdate
	static std::vector<SpinningTopEnemy*> hitEnemys;
	float deltaTime = Timer::Instance().DeltaTime();
	if (parry) {
		float parentSpeed = SpinningTopPlayerManager::Instance().GetPlayer(0)->GetData()->parryDamageIncrementSpeed;
		for (int i = 0; i < SpinningTopPlayerManager::Instance().GetPlayerCount(); i++) {
			StPlayerBase* player = SpinningTopPlayerManager::Instance().GetPlayer(i);
			PlayerData* data = player->GetData();
			player->parryDamageRadius += data->parryRadius / (parentSpeed) * deltaTime;
			if (player->parryDamageRadius > data->parryRadius) {
				player->parryDamageRadius = 0;
				parry = false;
				hitEnemys.clear();
			}
		}
	}

	if (parryGauge) {
		float parentSpeed = SpinningTopPlayerManager::Instance().GetPlayer(0)->GetData()->parryDamageIncrementSpeed;
		for (int i = 0; i < SpinningTopPlayerManager::Instance().GetPlayerCount(); i++) {
			StPlayerBase* player = SpinningTopPlayerManager::Instance().GetPlayer(i);
			PlayerData* data = player->GetData();
			player->parryDamageRadius += data->parryRadius / (parentSpeed) * deltaTime;
			if (player->parryDamageRadius > data->parryGaugeRadius) {
				player->parryDamageRadius = 0;
				parryGauge = false;
				hitEnemys.clear();
			}
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

			for (int i = 0; i < SpinningTopPlayerManager::Instance().GetPlayerCount(); i++) {
				StPlayerBase* player = SpinningTopPlayerManager::Instance().GetPlayer(i);
				PlayerData* data = player->GetData();
				XMVECTOR pPosVec = XMLoadFloat3(&player->GetPosition());
				XMVECTOR ePosVec = XMLoadFloat3(&enemy->GetPosition());
				float eRadius = enemy->GetRadius();
				float distance = XMVectorGetX(XMVector3Length(ePosVec - pPosVec));

				if ((eRadius + data->parryRadius > distance) && !parry) {
					parry = true;
					break;
				}

				if (parry && eRadius + player->parryDamageRadius > distance) {
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

					Collision::RepulsionSphereVsSphere(position, data->parryRadius, 1, enemy->GetPosition(), eRadius, 1, out1, out2);
					XMStoreFloat3(&out2, XMVector3Normalize(XMLoadFloat3(&out2)) * data->parryKnockback);

					if (!player->IsPlayer()) {
						XMVECTOR playerPosVec = XMLoadFloat3(&SpinningTopPlayerManager::Instance().GetPlayer(0)->GetPosition());
						XMVECTOR out2Vec = XMLoadFloat3(&out2);
						XMVECTOR power = XMVector3Length(out2Vec);
						XMStoreFloat3(&out2, (pPosVec - playerPosVec) * power);
					}
					enemy->SetVelocity(out2);
					enemy->ApplyDamage(1, 0);
#endif
				}
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

			for (int i = 0; i < SpinningTopPlayerManager::Instance().GetPlayerCount(); i++) {
				StPlayerBase* player = SpinningTopPlayerManager::Instance().GetPlayer(i);
				PlayerData* data = player->GetData();
				XMVECTOR pPosVec = XMLoadFloat3(&player->GetPosition());
				XMVECTOR ePosVec = XMLoadFloat3(&enemy->GetPosition());
				float eRadius = enemy->GetRadius();
				float distance = XMVectorGetX(XMVector3Length(ePosVec - pPosVec));

				if ((eRadius + data->parryGaugeRadius > distance) && !parryGauge) {
					parryGauge = true;
					break;
				}

				if (parryGauge && eRadius + player->parryDamageRadius > distance) {
					hitEnemys.push_back(enemy);

					XMFLOAT3 out1, out2;
					XMFLOAT3 eVel(enemy->GetVelocity());

					Collision::RepulsionSphereVsSphere(position, data->parryGaugeRadius, 1, enemy->GetPosition(), eRadius, 1, out1, out2);
					XMStoreFloat3(&out2, XMVector3Normalize(XMLoadFloat3(&out2)) * data->parryKnockback);

					if (!player->IsPlayer()) {
						XMVECTOR playerPosVec = XMLoadFloat3(&SpinningTopPlayerManager::Instance().GetPlayer(0)->GetPosition());
						XMVECTOR out2Vec = XMLoadFloat3(&out2);
						XMVECTOR power = XMVector3Length(out2Vec);
						XMStoreFloat3(&out2, (pPosVec - playerPosVec) * power);
					}
					enemy->SetVelocity(out2);
				}
			}
		}
	}

	//ちょっと適当
	if (GetInputMap<bool>("Attack") && !parry)parryCooldownCount = data->parryCooldown;
}

void StPlayerBase::RenderDebugPrimitive() {
	DebugPrimitive::Instance().AddSphere(position, radius, { 0,0,1,1 });
	DebugPrimitive::Instance().AddSphere(position, data->parryRadius, { 1,1,1,1 });
	if (rotateSpeed > data->parryGaugeConsumed)DebugPrimitive::Instance().AddSphere(position, data->parryGaugeRadius, { 1,0,1,1 });

	if (parry) DebugPrimitive::Instance().AddSphere(position, parryDamageRadius, { 0,1,0,1 });
	if (parryGauge) DebugPrimitive::Instance().AddSphere(position, parryDamageRadius, { 0,1,1,1 });
	if (parryCooldownCount > 0) DebugPrimitive::Instance().AddSphere(position, parryCooldownCount, { 1,0,0,1 });
}