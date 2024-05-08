#include "StPlayerBase.h"
#include "Library/Input/InputManager.h"
#include "Library/Timer.h"
#include "Library/3D/DebugPrimitive.h"

#include "SpinningTopPlayerManager.h"
#include "DamageTextManager.h"
#include "Library/Easing.h"
#include "Wave.h"
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
	debugValue["Move"] = movefloat2;

	//Attack
	bool enter = im.GetKeyPressed(Keyboard::Enter) || im.GetGamePadButtonPressed(GAMEPADBUTTON_STATE::a);
	bool attack = enter && parryCooldownCount <= 0;
	bool subAttack = enter && (rotateSpeed > data->parryGaugeConsumed);

	inputMap["SubAttack"] = subAttack;
	inputMap["Attack"] = attack && !subAttack;
}

void StPlayerBase::UpdateAttack() {
	// エフェクトの更新

	static const float parryEffectRangeChangePower = 3.0f;	// パリィ範囲を変更する力
	if (rotateSpeed > data->parryGaugeConsumed)
	{
		// 徐々にパリィ範囲のエフェクトのサイズを変更している
		DirectX::XMVECTOR PGR = DirectX::XMLoadFloat(&data->parryGaugeRadius);
		DirectX::XMVECTOR NOWR = DirectX::XMLoadFloat(&parryEffect->GetScale().x);
		NOWR = DirectX::XMVectorLerp(NOWR, PGR, parryEffectRangeChangePower * Timer::Instance().DeltaTime());
		float r;
		DirectX::XMStoreFloat(&r, NOWR);
		parryEffect->SetScale({ r, r, r });
	}
	else
	{
		// 徐々にパリィ範囲のエフェクトのサイズを変更している
		DirectX::XMVECTOR PR = DirectX::XMLoadFloat(&data->parryRadius);
		DirectX::XMVECTOR NOWR = DirectX::XMLoadFloat(&parryEffect->GetScale().x);
		NOWR = DirectX::XMVectorLerp(NOWR, PR, parryEffectRangeChangePower * Timer::Instance().DeltaTime());
		float r;
		DirectX::XMStoreFloat(&r, NOWR);
		parryEffect->SetScale({ r, r, r });
	}

	if (GetInputMap<bool>("Attack"))
	{
		domeEffect->StartEffect(data->parryRadius, { 0.62,1,1 });
	}
	if (GetInputMap<bool>("SubAttack"))
	{
		domeEffect->StartEffect(data->parryGaugeRadius);
	}


	//パルス状のUpdate
	static std::vector<SpinningTopEnemy*> hitEnemys;
	float deltaTime = Timer::Instance().DeltaTime();
	if (parry) {
		float parentSpeed = SpinningTopPlayerManager::Instance().GetPlayer(0)->GetData()->parryDamageIncrementSpeed;
		for (int i = 0; i < SpinningTopPlayerManager::Instance().GetPlayerCount(); i++) {
			StPlayerBase* player = SpinningTopPlayerManager::Instance().GetPlayer(i);
			PlayerData* data = player->GetData();
			player->parryDamageRadius +=  data->parryRadius/(parentSpeed) * deltaTime;
			debugValue[std::string("parryDamageRadius")+std::to_string(i)] = player->parryDamageRadius;
			if (player->parryDamageRadius > data->parryRadius) {
				parry = false;
			}
		}
		if (!parry) {
			for (int i = 0; i < SpinningTopPlayerManager::Instance().GetPlayerCount(); i++) {
				StPlayerBase* player = SpinningTopPlayerManager::Instance().GetPlayer(i);
				player->parryDamageRadius = 0;
			}
			hitEnemys.clear();
		}
	}

	if (parryGauge) {
		float parentSpeed = SpinningTopPlayerManager::Instance().GetPlayer(0)->GetData()->parryDamageIncrementSpeed;
		for (int i = 0; i < SpinningTopPlayerManager::Instance().GetPlayerCount(); i++) {
			StPlayerBase* player = SpinningTopPlayerManager::Instance().GetPlayer(i);
			PlayerData* data = player->GetData();
			player->parryDamageRadius += data->parryGaugeRadius / (parentSpeed) * deltaTime;
			debugValue[std::string("parryDamageRadius") + std::to_string(i)] = player->parryDamageRadius;
			if (player->parryDamageRadius > data->parryGaugeRadius) {
				parryGauge = false;
			}
		}
		if (!parryGauge) {
			for (int i = 0; i < SpinningTopPlayerManager::Instance().GetPlayerCount(); i++) {
				StPlayerBase* player = SpinningTopPlayerManager::Instance().GetPlayer(i);
				player->parryDamageRadius = 0;
			}
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


					float minDamage = 50;
					float maxDamage = 200;

					float damageAspect = 1.0f - (distance - player->radius - enemy->GetRadius()) / (player->data->parryRadius - player->radius - enemy->GetRadius());
					debugValue["dmgAsp"] = damageAspect;
					int dmg = (maxDamage - minDamage) * damageAspect + minDamage;

					totalHitScore += dmg;

					enemy->ApplyDamage(dmg, 0.0f);

					// ダメージ表示
					//int damage = 1;
					//std::wstring damageString = std::to_wstring(damage);
					//const TCHAR* damageTChar = damageString.c_str();
					//DamageText* damageText = new DamageText({ enemy->GetPosition().x, 1.0f, enemy->GetPosition().z }, damageTChar, { 0.62,1,1,1});
					DamageText* damageText = new DamageText({ enemy->GetPosition().x, 1.0f, enemy->GetPosition().z }, std::to_string(dmg), {1,0,0,1});
					DamageTextManager::Instance().Register(damageText);

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
				//parryGauge = true;

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

#if 0 ゲージパリィはダメージを与えない
					enemy->ApplyDamage(1, 0);

					// ダメージ表示
					int damage = 3;
					std::wstring damageString = std::to_wstring(damage);
					const TCHAR* damageTChar = damageString.c_str();
					DamageText* damageText = new DamageText({ enemy->GetPosition().x, 1.0f, enemy->GetPosition().z }, damageTChar, { 0.62,1,1,1 });
					DamageTextManager::Instance().Register(damageText);
#endif

					// ダウン状態に移行
					enemy->isDown = true;
				}
			}
		}
	}

	//ちょっと適当
	if (GetInputMap<bool>("Attack") && !parry)parryCooldownCount = data->parryCooldown;
}

void StPlayerBase::RenderDebugPrimitive() {
	//DebugPrimitive::Instance().AddSphere(position, radius, { 0,0,1,1 });
	DebugPrimitive::Instance().AddSphere(position, data->parryRadius, { 1,1,1,1 });
	if (rotateSpeed > data->parryGaugeConsumed)DebugPrimitive::Instance().AddSphere(position, data->parryGaugeRadius, { 1,0,1,1 });

	if (parry) DebugPrimitive::Instance().AddSphere(position, parryDamageRadius, { 0,1,0,1 });
	if (parryGauge) DebugPrimitive::Instance().AddSphere(position, parryDamageRadius, { 0,1,1,1 });
	if (parryCooldownCount > 0) DebugPrimitive::Instance().AddSphere(position, parryCooldownCount, { 1,0,0,1 });
}