#include "ObsParts.h"
#include "Library/3D/ResourceManager.h"
#include "Library/Timer.h"

ObsParts::ObsParts(const char* filePath)
{
	model = ResourceManager::Instance().LoadModelResource(filePath);

	// â¡ë¨ìxê›íË
	velocity =
	{
		static_cast<float>((rand() % (maxVelocity * 2)) - maxVelocity),
		0.0f,
		static_cast<float>((rand() % (maxVelocity * 2)) - maxVelocity)
	};

	// âÒì]ë¨ìxê›íË
	rotationSpeed = rand() % (maxRotationSpeed - minRotationSpeed) + minRotationSpeed;

	// íÔçRê›íË
	frictionPower = rand() % (maxFrictionPower - minFrictionPower) + minFrictionPower;

	// îºåaê›íË
	SetRadius(0.0f);

	// ìñÇΩÇËîªíË
	isCollision = false;
}


ObsParts::~ObsParts()
{
}


void ObsParts::Update()
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
		VELOCITY = DirectX::XMVectorSubtract(VELOCITY, SUB_VECLOCITY);												// â¡ë¨ìxÇÃå∏è≠èàóù
		VELOCITY_LENGTH = DirectX::XMVector3Length(VELOCITY);
		DirectX::XMStoreFloat(&velocityLength, VELOCITY_LENGTH);
		if (velocityLength < 0.0f)
		{
			VELOCITY = DirectX::XMVectorZero();
		}
	
		DirectX::XMStoreFloat3(&velocity, VELOCITY);
		DirectX::XMStoreFloat3(&position, POSITION);
	}
	
	// âÒì]
	if (rotationSpeed != 0)
	{
		DirectX::XMFLOAT3 ang = GetAngle();
		ang.y += rotationSpeed * Timer::Instance().DeltaTime();
		SetAngle(ang);
	
		float subRotationSpeed = rotationSpeed * frictionPower * Timer::Instance().DeltaTime();
		rotationSpeed -= subRotationSpeed;
		if (fabsf(rotationSpeed) < 0.01f) rotationSpeed = 0.0f;
	}

	// çsóÒçXêV
	UpdateTransform();
}
