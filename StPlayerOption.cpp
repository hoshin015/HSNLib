#include "StPlayerOption.h"
#include "Library/3D/DebugPrimitive.h"
#include "Library/3D/ResourceManager.h"
#include "Library/Input/InputManager.h"
#include "Library/ImGui/Include/imgui.h"
#include "Library/Timer.h"
#include "Library/3D/Camera.h"

#include "Collision.h"
#include "SpinningTopPlayerManager.h"

StPlayerOption::StPlayerOption(std::shared_ptr<SkinnedMesh> model, std::shared_ptr<PlayerData>& data) {
	this->model = model;
	this->data = data;
	isPlayer = false;
	SetScale({ .5f,.5f,.5f });

	// parryEffect
	parryEffect = std::make_unique<ParryEffect>(data->parryRadius);
	// domeEffect
	domeEffect = std::make_unique<DomeEffect>(data->parryRadius);
}

StPlayerOption::~StPlayerOption() {}

void StPlayerOption::Update() {
	try {
		parent = SpinningTopPlayerManager::Instance().GetPlayer(0);

	}
	catch (std::out_of_range& ex) {
		return;
	}

	// �G�t�F�N�g�̍X�V
	{
		static const float parryEffectRangeChangePower = 3.0f;	// �p���B�͈͂�ύX�����
		if (rotateSpeed > data->parryGaugeConsumed)
		{
			// ���X�Ƀp���B�͈͂̃G�t�F�N�g�̃T�C�Y��ύX���Ă���
			DirectX::XMVECTOR PGR = DirectX::XMLoadFloat(&data->parryGaugeRadius);
			DirectX::XMVECTOR NOWR = DirectX::XMLoadFloat(&parryEffect->GetScale().x);
			NOWR = DirectX::XMVectorLerp(NOWR, PGR, parryEffectRangeChangePower * Timer::Instance().DeltaTime());
			float r;
			DirectX::XMStoreFloat(&r, NOWR);
			parryEffect->SetScale({ r, r, r });
		}
		else
		{
			// ���X�Ƀp���B�͈͂̃G�t�F�N�g�̃T�C�Y��ύX���Ă���
			DirectX::XMVECTOR PR = DirectX::XMLoadFloat(&data->parryRadius);
			DirectX::XMVECTOR NOWR = DirectX::XMLoadFloat(&parryEffect->GetScale().x);
			NOWR = DirectX::XMVectorLerp(NOWR, PR, parryEffectRangeChangePower * Timer::Instance().DeltaTime());
			float r;
			DirectX::XMStoreFloat(&r, NOWR);
			parryEffect->SetScale({ r, r, r });
		}

		if (GetInputMap<bool>("Attack"))
		{
			domeEffect->StartEffect(data->parryRadius, { 0.62,1,1 });
		}
		if (GetInputMap<bool>("SubAttack"))
		{
			domeEffect->StartEffect(data->parryGaugeRadius);
		}
	}

	// parryEffect�X�V
	parryEffect->SetPosition({ position.x, 0.2f, position.z });
	parryEffect->Update();
	// domeEffect�X�V
	domeEffect->SetPosition({ position.x, 0.2f, position.z });
	domeEffect->Update();


	angle.y += 360 * Timer::Instance().DeltaTime();


	//UpdateAttack();

	// ���͍X�V����
	//UpdateVelocity();
	
	// �������͍X�V����
	UpdateHorizontalVelocity();
	// �����ړ��X�V����
	UpdateHorizontalMove();
	
	// ���G���ԍX�V
	UpdateInvincibleTimer();

	//UpdateAnimation();

	// �I�u�W�F�N�g�s��X�V
	UpdateTransform();
}

void StPlayerOption::Render() {
	model->Render(transform, { 1,1,1,1 }, &keyFrame);
	parryEffect->Render();
	domeEffect->Render();
}

void StPlayerOption::DrawDebugGui() {
	//RenderDebugPrimitive();
}
