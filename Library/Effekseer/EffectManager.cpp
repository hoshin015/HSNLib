#include "EffectManager.h"
#include "../Graphics/Graphics.h"
#include "../Timer.h"


// 初期化
void EffectManager::Initialize()
{
	Graphics& gfx = Graphics::Instance();

	// Effekseer レンダラ生成
	effekseerRenderer = EffekseerRendererDX11::Renderer::Create(gfx.device.Get(), gfx.deviceContext.Get(), 2048);

	// Effekseer マネージャー生成
	effekseerManager = Effekseer::Manager::Create(2048);

	// Effekseer レンダラの各種設定
	effekseerManager->SetSpriteRenderer(effekseerRenderer->CreateSpriteRenderer());
	effekseerManager->SetRibbonRenderer(effekseerRenderer->CreateRibbonRenderer());
	effekseerManager->SetRingRenderer(effekseerRenderer->CreateRingRenderer());
	effekseerManager->SetTrackRenderer(effekseerRenderer->CreateTrackRenderer());
	effekseerManager->SetModelRenderer(effekseerRenderer->CreateModelRenderer());

	// Effekseer 内でのローダーの設定
	effekseerManager->SetTextureLoader(effekseerRenderer->CreateTextureLoader());
	effekseerManager->SetModelLoader(effekseerRenderer->CreateModelLoader());
	effekseerManager->SetMaterialLoader(effekseerRenderer->CreateMaterialLoader());

	// Effekseer を左手座標系で計算する
	effekseerManager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);
}

// 終了化
void EffectManager::Finalize()
{
	// EffekseerManager などえはスマートポインタによって破棄されるので何もしない
}

// 更新処理
void EffectManager::Update()
{
	// エフェクト更新処理
	effekseerManager->Update(Timer::Instance().DeltaTime() * 60.0f);
}

// 描画処理
void EffectManager::Render(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection)
{
	// ビュー＆プロジェクション行列をEffekseerレンダラに設定
	effekseerRenderer->SetCameraMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&view));
	effekseerRenderer->SetProjectionMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(&projection));

	// Effekseer 描画開始
	effekseerRenderer->BeginRendering();

	// Effekseer 描画実行
	// マネージャー単位で描画するため描画順を制御する場合はマネージャーを複数個作成しDraw()関数を実行する順序で制御できそう
	effekseerManager->Draw();

	// Effekseer 描画終了
	effekseerRenderer->EndRendering();
}
