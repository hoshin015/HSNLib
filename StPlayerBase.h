#pragma once
#include <variant>
#include <map>

#include "SpinningTopBase.h"
#include "SpinningTopEnemyManager.h"
#include "StPlayerData.h"

class StPlayerBase :public SpinningTopBase {
public:
	virtual void Update() = 0;
	virtual void Render() = 0;
	virtual void DrawDebugGui() {}
	static void UpdateEDistance(const float& radius ,DirectX::XMFLOAT3& position);

	bool IsCoolDown() { return parryCooldownCount <= 0; }
	bool IsPlayer() { return isPlayer; }

private:
	//void UpdateRotate();
	//void UpdateMove();
	//void UpdateAttack();
	//void UpdateDamaged();
	//void RenderDebugPrimitive();

protected:
	virtual void Input() {}

	//�p�����[�^�͏��,�v���p�e�B�͉��ɏ����Ă�
	inline static std::vector<SpinningTopEnemy*> nearEnemy;

	PlayerData data;
	bool isPlayer;

	//�ړ�
	//float mobility;
	//float accel;
	//float slow;

	//��]
	float rotateSpeed;
	//float rotateMaxSpeed;

	//�p���B
	bool parry = false;
	float parryDamageRadius = 0;
	float parryCooldownCount = 0;

	bool parryGauge = false;
	float parryGaugeDamageRadius = 0;

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


	//����
	using InputVariant = std::variant<bool, int, float, DirectX::XMFLOAT2>;
	std::map<std::string, InputVariant> inputMap;

	template<typename T>
	const T& GetInputMap(std::string str) {
		T* result = std::get_if<T>(&inputMap[str]);
		return result ? *result : T();
	}

	//�f�o�b�O�p
	using debugVariant = std::variant<bool, int, float, DirectX::XMFLOAT2>;
	std::map<std::string, debugVariant> debugValue;

	template<typename T>
	T GetDebugValue(std::string str) {
		return std::get<T>(debugValue[str]);
	}


};