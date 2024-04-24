#include "StPlayerBase.h"
using namespace DirectX;

//TODO::��ԑ傫������̒��ɂ���Enemy���擾���� �����Ƃ������@���邩������Ȃ�
void StPlayerBase::UpdateEDistance(const float& radius, DirectX::XMFLOAT3& position) {
	nearEnemy.clear();
	SpinningTopEnemyManager& stEManager = SpinningTopEnemyManager::Instance();
	XMVECTOR pPosVec = XMLoadFloat3(&position);
	//float highestRadius = max(max(max(radius, data.parryRadius), parryDamageRadius), data.parryGaugeRadius);
	for (int i = 0; i < stEManager.GetEnemyCount(); i++) {
		SpinningTopEnemy* enemy = stEManager.GetEnemy(i);
		if (enemy->GetHealth() <= 0) continue;
		XMVECTOR ePosVec = XMLoadFloat3(&enemy->GetPosition());
		float eRadius = enemy->GetRadius();
		float distance = XMVectorGetX(XMVector3Length(ePosVec - pPosVec));

		if (radius + eRadius > distance) {
			nearEnemy.push_back(enemy);
		}
	}
}
