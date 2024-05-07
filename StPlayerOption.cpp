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

	// エフェクトの更新
	{
		static const float parryEffectRangeChangePower = 3.0f;	// パリィ範囲を変更する力
		if (rotateSpeed > data->parryGaugeConsumed)
		{
			// 徐々にパリィ範囲のエフェクトのサイズを変更している
			DirectX::XMVECTOR PGR = DirectX::XMLoadFloat(&data->parryGaugeRadius);
			DirectX::XMVECTOR NOWR = DirectX::XMLoadFloat(&parryEffect->GetScale().x);
			NOWR = DirectX::XMVectorLerp(NOWR, PGR, parryEffectRangeChangePower * Timer::Instance().DeltaTime());
			float r;
			DirectX::XMStoreFloat(&r, NOWR);
			parryEffect->SetScale({ r, r, r });
		}
		else
		{
			// 徐々にパリィ範囲のエフェクトのサイズを変更している
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

	// parryEffect更新
	parryEffect->SetPosition({ position.x, 0.2f, position.z });
	parryEffect->Update();
	// domeEffect更新
	domeEffect->SetPosition({ position.x, 0.2f, position.z });
	domeEffect->Update();


	angle.y += 360 * Timer::Instance().DeltaTime();


	//UpdateAttack();

	// 速力更新処理
	//UpdateVelocity();
	
	// 水平速力更新処理
	UpdateHorizontalVelocity();
	// 水平移動更新処理
	UpdateHorizontalMove();
	
	// 無敵時間更新
	UpdateInvincibleTimer();

	//UpdateAnimation();

	// オブジェクト行列更新
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
