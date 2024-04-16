#include "StPlayer.h"
#include "Library/3D/DebugPrimitive.h"
#include "Library/3D/ResourceManager.h"

StPlayer::StPlayer() {
	model = ResourceManager::Instance().LoadModelResource("Data/Fbx/Jummo/Jummo.fbx");
}

StPlayer::~StPlayer() {}

void StPlayer::Update() {
	// 速力更新処理
	UpdateVelocity();
	// 無敵時間更新
	UpdateInvincibleTimer();

	UpdateAnimation();

	// オブジェクト行列更新
	UpdateTransform();
}

void StPlayer::Render() {
	model->Render(transform, { 0,1,1,1 }, &keyFrame);
}

void StPlayer::OnLanding() {}

void StPlayer::OnDamaged() {}

void StPlayer::OnDead() {}
