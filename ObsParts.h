#pragma once
#include "Obstacle.h"

class ObsParts : public Obstacle
{
public:
	ObsParts(const char* filePath, bool pParts = false);
	~ObsParts() override;

	void Update() override;

private:
	enum class STATE
	{
		BREAK,		// ”j‰ó‚³‚ê‚Ä¶¬‚³‚ê‚½
		WAIT,		// ’Êí
		GO_PLAYER,	// ƒvƒŒƒCƒ„[‚ÉŒü‚©‚Á‚ÄˆÚ“®
	};
	STATE state = STATE::BREAK;

	void TranslationBreak();
	void UpdateBreak();
	void TranslationWait();
	void UpdateWait();
	void TranslationGoPlayer();
	void UpdateGoPlayer();

	// ’ïR‚ğ—^‚¦‚é
	void Friction();

	DirectX::XMFLOAT3 velocity = { 0,0,0 };
	float frictionPower = 5.0f;

	float rotationSpeed = 0;	// 1•b‚É‰ñ“]‚·‚éŠp“x

	int maxVelocity = 20.0f;
	int maxFrictionPower = 5.0f;
	int minFrictionPower = 3.0f;
	int maxRotationSpeed = 120.0f;
	int minRotationSpeed = 30.0f;

	float canGoPlayerTimer = 0.0f;
	float canGoPlayerTime = 1.0f;

	float toPlayerDistance = 10.0f;	// ƒvƒŒƒCƒ„[‚ÉŒü‚©‚¤‚Æ”»’f‚·‚é‹——£
	float goPlayerSpeed = 20.0f;

	bool playerParts = false;
};