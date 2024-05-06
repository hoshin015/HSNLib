#include "StMenuPlayer.h"
#include "Library/Timer.h"
#include "Library/Input/InputManager.h"
#include "Library/3D/ResourceManager.h"
#include "DataManager.h"
using namespace DirectX;

StMenuPlayer::StMenuPlayer() {
	model = ResourceManager::Instance().LoadModelResource("Data/Fbx/StPlayer/Main/StPlayer.fbx");
	data = std::make_unique<PlayerData>();
	DataManager::Instance().LoadPlayerData(data.get(), 1);
}

void StMenuPlayer::Update() {
	InputManager& im = InputManager::Instance();
	//Move
	XMFLOAT2 movefloat2;
	movefloat2.x = im.GetKeyPress(Keyboard::A) - im.GetKeyPress(Keyboard::D);
	movefloat2.y = im.GetKeyPress(Keyboard::S) - im.GetKeyPress(Keyboard::W);
	XMStoreFloat2(&movefloat2, XMVector2Normalize(XMLoadFloat2(&movefloat2)));

	if (im.IsGamePadConnected()) {
		movefloat2.x = -im.GetThumSticksLeftX();
		movefloat2.y = -im.GetThumSticksLeftY();
	}

	float deltaTime = Timer::Instance().DeltaTime();
	XMFLOAT2 input = movefloat2;
	XMFLOAT2 moveDirection;
	XMVECTOR inputVec = XMLoadFloat2(&input);
	XMVECTOR direction = XMVectorSet(velocity.x, velocity.z, 0, 0);
	float speed = XMVectorGetX(XMVector2Length(direction));
	direction = XMVector2Normalize(direction);

	//コントローラー用
	if (XMVectorGetX(XMVector2Length(inputVec)) > 1) {
		inputVec = XMVector2Normalize(inputVec);
	}

	//速度計算
	if (fabsf(input.x) > 0.00001f || fabsf(input.y) > 0.00001f) {
		float dot = XMVectorGetX(XMVector2Dot(direction, inputVec));
		if (speed == 0) dot = 1;
		speed += XMVectorGetX(XMVector2Length(inputVec) * dot) * data->accel * deltaTime;
		speed = max(0.f, speed);
	}
	else speed = max(0.0f, speed - (data->slow * deltaTime));

	speed -= speed * 0.998f * deltaTime;
	maxMoveSpeed = (data->accel + (data->accel * 0.002f));
	speed = min(speed, maxMoveSpeed);

	//機動性の計算
	if (speed < 0.0000001) direction = {};
	float lerp = min(max(0, (speed / maxMoveSpeed * data->mobility) + (1 - data->mobility)), 0.95f);
	direction = XMVectorLerp(direction, XMVectorLerp(inputVec, direction, lerp), 30 * deltaTime);
	direction = XMVector2Normalize(direction);
	XMStoreFloat2(&moveDirection, direction);

	velocity.x = moveDirection.x * speed;
	velocity.z = moveDirection.y * speed;

	angle.y += 360 * 2 * Timer::Instance().DeltaTime();
	if (360 < angle.y || angle.y < -360)angle.y += angle.y > 0 ? -360 : 360;

	// 速力更新処理
	UpdateVelocity();
	// 無敵時間更新
	UpdateInvincibleTimer();

	//UpdateAnimation();


	if (position.x < -15.5f) position.x = -15.5f;
	if (position.x >  15.5f) position.x =  15.5f;
	if (position.z < -15) position.z = -15;
	if (position.z >  11) position.z =  11;
	// オブジェクト行列更新
	UpdateTransform();


}

void StMenuPlayer::Render() {
	model->Render(transform, { 1,1,1,1 }, nullptr);
}
