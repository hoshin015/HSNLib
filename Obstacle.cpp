#include "Obstacle.h"
#include "Library/Graphics/Graphics.h"
#include "Library/3D/DebugPrimitive.h"
#include "Library/Timer.h"

Obstacle::Obstacle(const char* name, bool isCol)
{
	model = new SkinnedMesh(name);
	this->isCollision = isCol;
}

Obstacle::~Obstacle()
{
	delete model;
}

// çXêVèàóù
void Obstacle::Update()
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


	UpdateTransform();
}

// ï`âÊèàóù
void Obstacle::Render()
{
	// --- Graphics éÊìæ ---
	Graphics& gfx = Graphics::Instance();

	// rasterizer ê›íË
	gfx.SetRasterizer(RASTERIZER_STATE::CLOCK_TRUE_SOLID);

	model->Render(transform, { 1, 1, 1, 1 }, 0);
}

void Obstacle::DrawDebugPrimitive()
{
	//DebugPrimitive::Instance().AddSphere(position, radius, { 0,0,0,1 });
}

bool Obstacle::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	return Collision::IntersectRayVsModel(start, end, model, transform, hit);
	//return Collision::IntersectRayVsModel(start, end, collisionModel, hit);
}
void Obstacle::UpdateTransform()
{
	const float scaleFactor = model->scaleFactors[model->fbxUnit];
	DirectX::XMMATRIX C = DirectX::XMLoadFloat4x4(&model->coordinateSystemTransform[model->coordinateSystemIndex]) * DirectX::XMMatrixScaling(scaleFactor, scaleFactor, scaleFactor);

	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX W = C * S * R * T;
	DirectX::XMStoreFloat4x4(&transform, W);
}
