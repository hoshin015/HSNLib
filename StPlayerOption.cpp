#include "StPlayerOption.h"
#include "Library/3D/DebugPrimitive.h"
#include "Library/3D/ResourceManager.h"
#include "Library/Input/InputManager.h"
#include "Library/ImGui/Include/imgui.h"
#include "Library/Timer.h"
#include "Library/3D/Camera.h"

#include "Collision.h"
#include "SpinningTopPlayerManager.h"

StPlayerOption::StPlayerOption(std::shared_ptr<SkinnedMesh> model, PlayerData& data) {
	this->model = model;
	this->data = data;
	isPlayer = false;
	SetScale({ .5f,.5f,.5f });

	try {
		parent = SpinningTopPlayerManager::Instance().GetPlayer(0);

	}
	catch (std::out_of_range& ex) {
		parent = nullptr;
	}
}

StPlayerOption::~StPlayerOption() {}

void StPlayerOption::Update() {
	if (!parent) return;
	angle.y += 360 * Timer::Instance().DeltaTime();
	UpdateAttack();

	// ���͍X�V����
	UpdateVelocity();
	// ���G���ԍX�V
	UpdateInvincibleTimer();

	//UpdateAnimation();

	// �I�u�W�F�N�g�s��X�V
	UpdateTransform();
}

void StPlayerOption::Render() {
	model->Render(transform, { 0,1,0,1 }, &keyFrame);
}

void StPlayerOption::DrawDebugGui() {
	RenderDebugPrimitive();
}
