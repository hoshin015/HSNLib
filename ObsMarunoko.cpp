#include "ObsMarunoko.h"
#include "Library/3D/ResourceManager.h"
#include "Library/Timer.h"

ObsMarunoko::ObsMarunoko(const char* filePath)
{
	model = ResourceManager::Instance().LoadModelResource(filePath);

	hitDamae = 1;

	SetRadius(3.0f);
}

ObsMarunoko::~ObsMarunoko()
{
}

void ObsMarunoko::Update()
{
	// âÒì]
	DirectX::XMFLOAT3 ang = GetAngle();
	ang.y += rotationSpeed * Timer::Instance().DeltaTime();
	SetAngle(ang);

	// çsóÒçXêV
	UpdateTransform();
}
