#include "StPlayerBase.h"
#include "Library/Input/InputManager.h"
using namespace DirectX;

//TODO::ˆê”Ô‘å‚«‚¢”»’è‚Ì’†‚É‚¢‚éEnemy‚ðŽæ“¾‚·‚é ‚à‚Á‚Æ‚¢‚¢•û–@‚ ‚é‚©‚à‚µ‚ê‚È‚¢
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
			float highestRadius = max(max(max(player->radius, player->data.parryRadius), player->parryDamageRadius), player->data.parryGaugeRadius);
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
	subAttack = subAttack && (rotateSpeed > data.parryGaugeConsumed);
	inputMap["SubAttack"] = subAttack;
}
