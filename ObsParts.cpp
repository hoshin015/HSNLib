#include "ObsParts.h"
#include "Library/3D/ResourceManager.h"
#include "Library/Timer.h"
#include "SpinningTopPlayerManager.h"
#include "ObstacleManager.h"

ObsParts::ObsParts(const char* filePath)
{
	model = ResourceManager::Instance().LoadModelResource(filePath);

	// 加速度設定
	velocity =
	{
		static_cast<float>((rand() % (maxVelocity * 2)) - maxVelocity),
		0.0f,
		static_cast<float>((rand() % (maxVelocity * 2)) - maxVelocity)
	};

	// 回転速度設定
	rotationSpeed = rand() % (maxRotationSpeed - minRotationSpeed) + minRotationSpeed;

	// 抵抗設定
	frictionPower = rand() % (maxFrictionPower - minFrictionPower) + minFrictionPower;

	// 半径設定
	SetRadius(0.0f);

	// 当たり判定
	isCollision = false;

	color.w = 0.5f;
}


ObsParts::~ObsParts()
{

}


void ObsParts::Update()
{
	switch (state)
	{
	case ObsParts::STATE::BREAK:
		UpdateBreak();
		break;
	case ObsParts::STATE::WAIT:
		UpdateWait();
		break;
	case ObsParts::STATE::GO_PLAYER:
		UpdateGoPlayer();
		break;
	}

	// 行列更新
	UpdateTransform();
}

void ObsParts::TranslationBreak()
{
	state = STATE::BREAK;
}

void ObsParts::UpdateBreak()
{
	canGoPlayerTimer += Timer::Instance().DeltaTime();
	if (canGoPlayerTimer > canGoPlayerTime)
	{
		TranslationWait();
	}

	Friction();
}

void ObsParts::TranslationWait()
{
	rotationSpeed = 0.0f;
	velocity = { 0,0,0 };

	state = STATE::WAIT;
}

void ObsParts::UpdateWait()
{
	DirectX::XMVECTOR PL = DirectX::XMLoadFloat3(&SpinningTopPlayerManager::Instance().GetPlayer(0)->GetPosition());
	DirectX::XMVECTOR POS = DirectX::XMLoadFloat3(&position);
	PL = DirectX::XMVectorSubtract(PL, POS);
	DirectX::XMVECTOR LENGTH = DirectX::XMVector3Length(PL);
	float length;
	DirectX::XMStoreFloat(&length, LENGTH);

	if (length < toPlayerDistance)
	{
		// ステート変更
		TranslationGoPlayer();
	}

	Friction();
}

void ObsParts::TranslationGoPlayer()
{
	state = STATE::GO_PLAYER;
}

void ObsParts::UpdateGoPlayer()
{
	DirectX::XMVECTOR PL = DirectX::XMLoadFloat3(&SpinningTopPlayerManager::Instance().GetPlayer(0)->GetPosition());
	DirectX::XMVECTOR POS = DirectX::XMLoadFloat3(&position);
	DirectX::XMVECTOR V = DirectX::XMVectorSubtract(PL, POS);
	DirectX::XMVECTOR LENGTH = DirectX::XMVector3Length(V);
	float length;
	DirectX::XMStoreFloat(&length, LENGTH);
	V = DirectX::XMVector3Normalize(V);
	V = DirectX::XMVectorScale(V, Timer::Instance().DeltaTime() * goPlayerSpeed);
	DirectX::XMStoreFloat3(&velocity, V);

	position.x += velocity.x;
	position.y += velocity.y;
	position.z += velocity.z;

	if (length < 0.1f)
	{
		ObstacleManager::Instance().Remove(this);
	}
}

void ObsParts::Friction()
{
	DirectX::XMVECTOR VELOCITY = DirectX::XMLoadFloat3(&velocity);
	DirectX::XMVECTOR VELOCITY_LENGTH = DirectX::XMVector3Length(VELOCITY);
	float velocityLength;
	DirectX::XMStoreFloat(&velocityLength, VELOCITY_LENGTH);

	if (velocityLength > 0.0f)
	{
		DirectX::XMVECTOR POSITION = DirectX::XMLoadFloat3(&position);
		POSITION = DirectX::XMVectorAdd(POSITION, DirectX::XMVectorScale(VELOCITY, Timer::Instance().DeltaTime()));
		DirectX::XMVECTOR SUB_VECLOCITY = DirectX::XMVectorScale(VELOCITY, frictionPower * Timer::Instance().DeltaTime());
		VELOCITY = DirectX::XMVectorSubtract(VELOCITY, SUB_VECLOCITY);												// 加速度の減少処理
		VELOCITY_LENGTH = DirectX::XMVector3Length(VELOCITY);
		DirectX::XMStoreFloat(&velocityLength, VELOCITY_LENGTH);
		if (velocityLength < 0.0f)
		{
			VELOCITY = DirectX::XMVectorZero();
		}

		DirectX::XMStoreFloat3(&velocity, VELOCITY);
		DirectX::XMStoreFloat3(&position, POSITION);
	}

	// 回転
	if (rotationSpeed != 0)
	{
		DirectX::XMFLOAT3 ang = GetAngle();
		ang.y += rotationSpeed * Timer::Instance().DeltaTime();
		SetAngle(ang);

		float subRotationSpeed = rotationSpeed * frictionPower * Timer::Instance().DeltaTime();
		rotationSpeed -= subRotationSpeed;
		if (fabsf(rotationSpeed) < 0.01f)
		{
			rotationSpeed = 0.0f;
		}
	}
}


