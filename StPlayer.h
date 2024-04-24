#pragma once
#include <variant>
#include <map>

#include "StPlayerBase.h"
#include "SpinningTopEnemyManager.h"
#include "StPlayerOption.h"
#include "StPlayerData.h"

class StPlayer :public StPlayerBase
{
public:
	StPlayer();
	~StPlayer() override;

	void Update() override;
	void Render() override;
	void DrawDebugGui() override;

private:
	void UpdateRotate();
	void UpdateMove();
	void UpdateAttack();
	void UpdateDamaged();

	void RenderDebugPrimitive();

protected:
	void Input() override;
	void OnLanding() override;
	void OnDamaged() override;
	void OnDead() override;

private:
	//�p�����[�^�͏��,�v���p�e�B�͉��ɏ����Ă�
	//std::vector<SpinningTopEnemy*> nearEnemy;

	//PlayerData data;

	//�ړ�
	//float mobility;
	//float accel;
	//float slow;

	//��]
	//float rotateSpeed;
	//float rotateMaxSpeed;

	//�p���B
	//bool parry = false;
	//float parryDamageRadius = 0;
	//float parryCooldownCount = 0;

	//bool parryGauge = false;
	//float parryGaugeDamageRadius = 0;

	//float parryRadius;
	//float parryDamageMaxRadius;
	//float parryDamageIncrementSpeed;
	//float parryCooldown;
	//float parryKnockback;

	//float parryGaugeRadius;
	//float parryGaugeConsumed;
	//float parryGaugeDamageMaxRadius;

	//�_���[�W
	//float damagedKnockback;

	//�q�@
	std::shared_ptr<SkinnedMesh> childModel;
	//std::vector<std::shared_ptr<StPlayerOption>> option;

	//����
	//using InputVariant = std::variant<bool, int, float, DirectX::XMFLOAT2>;
	//std::map<std::string, InputVariant> inputMap;

	//template<typename T>
	//const T& GetInputMap(std::string str) {
	//	T* result = std::get_if<T>(&inputMap[str]);
	//	return result ? *result : T();
	//}

	//�f�o�b�O�p
	//using debugVariant = std::variant<bool, int, float ,DirectX::XMFLOAT2>;
	//std::map<std::string, debugVariant> debugValue;

	//template<typename T>
	//T GetDebugValue(std::string str) {
	//	return std::get<T>(debugValue[str]);
	//}


};

