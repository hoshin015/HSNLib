#pragma once

struct PlayerData {
	float radius = .5f;

	//�ړ�
	float mobility = 1;
	float accel = 10;
	float slow = 1;

	//��]
	float rotateInitialSpeed = 2;
	float rotateMaxSpeed = 100;

	//�p���B
	float parryRadius = 1;
	float parryDamageMaxRadius = 10;
	float parryDamageIncrementSpeed = 100;
	float parryCooldown = 1;
	float parryKnockback = 30;

	float parryGaugeRadius = 10;
	float parryGaugeConsumed = 1;
	float parryGaugeDamageMaxRadius = 10;

	//�_���[�W
	float damagedKnockback = 15;
};