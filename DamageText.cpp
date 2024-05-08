#include "DamageText.h"
#include "DamageTextManager.h"
#include "Library/Text/DispString.h"
#include "Library/Graphics/Graphics.h"
#include "Library/3D/Camera.h"
#include "Library/Timer.h"

// 更新処理
void DamageText::Update()
{
	switch (state)
	{
	case DamageText::State::Idle:
		UpdateIdleState();
		break;
	case DamageText::State::fadeOut:
		UpdateFadeOutState();
		break;
	}

	// 座標更新
	UpdatePosition();
}

// 描画処理
void DamageText::Render()
{
	float fontSize = 40;
	DamageTextManager::Instance().textSpr->SprTextOut(strText, { drawPosition.x - fontSize*0.5f, drawPosition.y - fontSize*0.5f }, { fontSize, fontSize });
	//DispString::Instance().Draw(text, { drawPosition.x, drawPosition.y }, size, TEXT_ALIGN::MIDDLE, color, false, { 0,0,0,1 }, 2);
}

// 破棄
void DamageText::Destroy()
{
	DamageTextManager::Instance().Remove(this);
}

// 座標更新
void DamageText::UpdatePosition()
{
	// ---- ポジション更新 ---

	// Graphics 取得 
	Graphics& gfx = Graphics::Instance();

	// ビューポート
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	gfx.deviceContext->RSGetViewports(&numViewports, &viewport);

	// 変換行列
	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&Camera::Instance().GetView());
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&Camera::Instance().GetProjection());
	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

	// エネミー頭上の座標取得
	DirectX::XMFLOAT3 worldPosition = position;
	worldPosition.y += addY;
	DirectX::XMVECTOR WorldPosition = DirectX::XMLoadFloat3(&worldPosition);

	// ワールド座標からスクリーン座標への変換
	DirectX::XMVECTOR ScreenPosition = DirectX::XMVector3Project(
		WorldPosition,
		viewport.TopLeftX,
		viewport.TopLeftY,
		viewport.Width,
		viewport.Height,
		viewport.MinDepth,
		viewport.MaxDepth,
		Projection,
		View,
		World
	);

	// スクリーン座標
	DirectX::XMFLOAT2 screenPosition;
	DirectX::XMStoreFloat2(&drawPosition, ScreenPosition);
}


//==========================================================================
//
//　ステート処理
// 
//==========================================================================

// 待機ステートへ遷移
void DamageText::TransitionIdleState()
{
	timer = 0.0f;

	state = State::Idle;
}

// 待機ステート更新処理
void DamageText::UpdateIdleState()
{
	// タイマー処理
	timer += Timer::Instance().DeltaTime();
	if (timer >= idleTime)
	{
		TransitionFadeOutState();
		return;
	}
}

// フェードアウトステートへ遷移
void DamageText::TransitionFadeOutState()
{
	timer = 0.0f;

	state = State::fadeOut;
}

// フェードアウトステート更新処理
void DamageText::UpdateFadeOutState()
{
	// タイマー処理
	timer += Timer::Instance().DeltaTime();
	if (timer >= fadeOutTime)
	{
		Destroy();
		return;
	}

	addY += 4 * Timer::Instance().DeltaTime();
}
