#include "ObsMarunoko.h"
#include "Library/3D/ResourceManager.h"
#include "Library/Timer.h"
#include "Library/Graphics/Graphics.h"

ObsMarunoko::ObsMarunoko(const char* filePath, int type, float speed)
{
	model = ResourceManager::Instance().LoadModelResource(filePath);
	this->type = static_cast<MarunokoType>(type);
	moveSpeed = speed;

	hitDamae = 1;
	isWaveDestory = true;

	SetRadius(3.0f);

	switch (type)
	{
	case NO_MOVE:
	{
		break;
	}
	case HORIZONTAL:
	{
		velocity.x = (rand() % 2) ? moveSpeed : -moveSpeed;
		break;
	}
	case VERTICAL:
	{
		velocity.z = (rand() % 2) ? moveSpeed : -moveSpeed;
		break;
	}
	}

	spawnEffect = std::make_unique<EnemySpawnEffect>(7.0f);
}

ObsMarunoko::~ObsMarunoko()
{
}

void ObsMarunoko::Update()
{
	if (!isSpawnFinish)
	{
		spawnWiatTimer += Timer::Instance().DeltaTime();
		if (spawnWiatTimer > spawnWaitTime)
		{
			isSpawnFinish = true;
			position = spawnPos;
		}

		spawnEffect->Update();
		// 行列更新
		UpdateTransform();
		return;
	}
	


	// 回転
	DirectX::XMFLOAT3 ang = GetAngle();
	ang.y += rotationSpeed * Timer::Instance().DeltaTime();
	SetAngle(ang);

	switch (type)
	{
	case NO_MOVE:
	{
		break;
	}
	case HORIZONTAL:
	{
		if (position.x > 50) velocity.x = -1 * moveSpeed;
		if (position.x < -50) velocity.x = 1 * moveSpeed;
		break;
	}
	case VERTICAL:
	{
		if (position.z > 50) velocity.z = -1 * moveSpeed;
		if (position.z < -50) velocity.z = 1 * moveSpeed;
		break;
	}
	}

	position.x += velocity.x * Timer::Instance().DeltaTime();
	position.y += velocity.y * Timer::Instance().DeltaTime();
	position.z += velocity.z * Timer::Instance().DeltaTime();

	// 行列更新
	UpdateTransform();
}

void ObsMarunoko::Render()
{
	// --- Graphics 取得 ---
	Graphics& gfx = Graphics::Instance();

	// rasterizer 設定
	gfx.SetRasterizer(RASTERIZER_STATE::CLOCK_TRUE_SOLID);

	if(!isSpawnFinish) spawnEffect->Render();
	
	model->Render(transform, color, NULL);
}
