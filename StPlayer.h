#pragma once
#include <variant>
#include <map>

#include "SpinningTopBase.h"
#include "SpinningTopEnemyManager.h"

class StPlayer :public SpinningTopBase
{
public:
	StPlayer();
	~StPlayer() override;

	void Update();
	void Render();
	void DrawDebugGui();

	bool IsCoolDown() { return parryCooldownCount <= 0; }

private:
	void Input();
	void UpdateMove();
	void UpdateAttack();
	void UpdateDamage();
	void UpdateEDistance();

protected:
	void OnLanding() override;
	void OnDamaged() override;
	void OnDead() override;

private:
	//�p�����[�^�͏��,�v���p�e�B�͉��ɏ����Ă�
	std::vector<SpinningTopEnemy*> nearEnemy;

	//�ړ�
	float mobility;
	float accel;
	float slow;

	//��]
	float rotateIncrementSpeed;
	float rotateIncrement;

	//�p���B
	bool parry = false;
	float parryDamageRadius = 0;
	float parryCooldownCount = 0;

	float parryRadius;
	float parryDamageMaxRadius;
	float parryDamageIncrementSpeed;
	float parryCooldown;
	float parryKnockback;

	//�_���[�W
	float damagedKnockback;

	//�q�@

	//����
	using InputVariant = std::variant<bool, int, float, DirectX::XMFLOAT2>;
	std::map<std::string, InputVariant> inputMap;

	template<typename T>
	T GetInputMap(std::string str) {
		return std::get<T>(inputMap[str]);
	}

	//�f�o�b�O�p
	using debugVariant = std::variant<bool, int, float ,DirectX::XMFLOAT2>;
	std::map<std::string, debugVariant> debugValue;

	template<typename T>
	T GetDebugValue(std::string str) {
		return std::get<T>(debugValue[str]);
	}


};

