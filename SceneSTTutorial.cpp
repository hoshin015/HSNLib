#include "SceneSTTutorial.h"
#include "SceneManager.h"
#include "Library/Input/InputManager.h"
#include "Library/ImGui/Include/imgui.h"
#include "Library/Text/DispString.h"
#include "Library/Text/Text.h"
#include "Library/Timer.h"
#include "Library/Input/InputManager.h"
#include "Library/Easing.h"
#include "Library/FrameWork.h"


#include "StageManager.h"
#include "LightManager.h"
#include "SpinningTopEnemyManager.h"
#include "SpinningTopPlayerManager.h"
#include "DataManager.h"
#include "ObstacleManager.h"
#include "ObsStaticObj.h"
#include "ObsMarunoko.h"
#include "DamageTextManager.h"

void SceneSTTutorial::Initialize() {
	VideoManager::Instance().LoadFile(PARRY, nullptr);
	sprite.emplace_back(std::make_unique<Sprite>(L"Data/Texture/Enter.png"));
	sprite.emplace_back(std::make_unique<Sprite>(L"Data/Texture/Back.png"));
	videoUI.SetVideo(&VideoManager::Instance().GetVideo(PARRY));
	videoUI.SetTextSprite(sprite[PARRY].get());
	VideoManager::Instance().Play(PARRY,true);
	stateMap["StopUpdate"] = true;
	stateMap["DrawVideo"] = false;
	stateMap["TutorialStart"] = true;

	DirectX::XMFLOAT2 texSize = sprite[1]->GetTexSize();
	sSize = texSize;
	sPos = { -sSize.x * .5f,Framework::Instance().windowHeight * .5f };

	DataManager::Instance().LoadEnemyData(enemyData);
	DataManager::Instance().LoadSpawnEreaData();

	// プレイヤー初期化
	StPlayerBase* player = new StPlayer();
	SpinningTopPlayerManager::Instance().Register(player);


	// Cylinder
	for (int i = 0; i < 3; i++) {
		ObsStaticObj* obstacle = new_ ObsStaticObj("Data/FBX/cylinder/cylinder.fbx");
		obstacle->SetPosition({ i * 30.0f - 45.0f, 0, -6.0f });
		ObstacleManager::Instance().Register(obstacle);
	}

	// マルのこ
	for (int i = 0; i < 3; i++) {
		ObsMarunoko* obstacle = new_ ObsMarunoko("Data/FBX/StMarunoko/StMarunoko.fbx");
		obstacle->SetPosition({ i * 30.0f - 45.0f, 0, 6.0f });
		ObstacleManager::Instance().Register(obstacle);
	}

	// ステージ初期化
	StageManager& stageManager = StageManager::Instance();
	stageMain = std::make_unique<StageContext>();
	stageManager.Register(stageMain.get());

	// ライト初期化
	Light* directionLight = new Light(LightType::Directional);
	directionLight->SetDirection(DirectX::XMFLOAT3(0.5, -1, -1));
	directionLight->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
	LightManager::Instance().Register(directionLight);

	// スカイマップ
	skyMap = std::make_unique<SkyMap>(L"Data/Texture/kloppenheim_05_puresky_4k.hdr");

	// カメラ初期設定
	Camera::Instance().SetLookAt(
		DirectX::XMFLOAT3(0, 90, 60),		// カメラ座標
		DirectX::XMFLOAT3(-90, 0, -30),		// ターゲット(設定しても意味ない)
		DirectX::XMFLOAT3(0, 1, 0)			// 上方向ベクトル
	);
	Camera::Instance().SetAngle({ DirectX::XMConvertToRadians(60), DirectX::XMConvertToRadians(180), 0 });

	Camera::Instance().cameraType = Camera::CAMERA::TargetStPlayer;
}

void SceneSTTutorial::Finalize() {
	VideoManager::Instance().Clear();

	DamageTextManager::Instance().Clear();

	SpinningTopPlayerManager::Instance().Clear();

	StageManager::Instance().Clear();

	LightManager::Instance().Clear();

	SpinningTopEnemyManager::Instance().Clear();

	ObstacleManager::Instance().Clear();
}

void SceneSTTutorial::Update() {
	UpdateState();
	Camera::Instance().Update();
	VideoManager::Instance().Update();

	if(!stateMap["StopUpdate"]) {
		// カメラコントローラー更新処理

		SpinningTopPlayerManager::Instance().Update();

		StageManager::Instance().Update();

		SpinningTopEnemyManager::Instance().Update();

		ObstacleManager::Instance().Update();

		DamageTextManager::Instance().Update();
	}
}

void SceneSTTutorial::UpdateState() {
	InputManager& im = InputManager::Instance();
	float width = Framework::Instance().windowWidth;
	float height = Framework::Instance().windowHeight;
	float wRatio = width / 1280;
	float hRatio = height / 720;
	float sRatio = wRatio * hRatio;

	if (stateMap["TutorialStart"]) {
		constexpr float kHalfTime = 1.5f;

		time += Timer::Instance().DeltaTime();
		if (time < kHalfTime)ease = (sSize.x + width) * 0.5f * Easing(time, kHalfTime, easeCubic, easeOut);
		else ease = (sSize.x + width) * 0.5f * (1 + Easing(time - kHalfTime, kHalfTime, easeCubic, easeIn));
		sPos.x = -sSize.x * .5f + ease;

		if (time > kHalfTime * 2) {
			stateMap["TutorialStart"] = false;
			stateMap["FirstStart"] = true;
			stateMap["StopUpdate"] = false;
			time = 0;
		}
		return;
	}
	else if(stateMap["FirstStart"]) {
		time += Timer::Instance().DeltaTime();
		if (time > 1.0f) {
			stateMap["DrawVideo"] = true;
			stateMap["StopUpdate"] = true;
			stateMap["FirstStart"] = false;
			time = 0;
		}
	}

	if (stateMap["StopUpdate"]) {
		videoUI.SetPosition({ width * .5f,height * .5f });
		videoUI.SetSize({ 640 * wRatio,400 * hRatio });
		if (im.GetKeyPressed(Keyboard::Enter)) {
			stateMap["StopUpdate"] = false;
		}
	}
	else {
		videoUI.SetPosition({ width * .87f,height * .15f });
		videoUI.SetSize({ 256 * wRatio,160 * hRatio });
	}

	if (tarm == 0) {
		tState++;
		if (tState == SPRITESTART - 1) {
			stateMap["TutorialEnd"] = true;
			return;
		}

		videoUI.SetVideo(&VideoManager::Instance().GetVideo(tState));
		videoUI.SetTextSprite(sprite[tState].get());
		stateMap["StopUpdate"] = true;
	}
}

void SceneSTTutorial::Render() {
	// --- Graphics 取得 ---
	Graphics& gfx = Graphics::Instance();

	// shadowMap
	{
		gfx.SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);

		gfx.shadowBuffer->Clear();
		gfx.shadowBuffer->Activate();


		// カメラパラメータ設定
		{
			// 平行光源からカメラ位置を作成し、そこから原点の位置を見るように視線行列を生成
			DirectX::XMFLOAT3 dir = LightManager::Instance().GetLight(0)->GetDirection();
			DirectX::XMVECTOR LightPosition = DirectX::XMLoadFloat3(&dir);
			LightPosition = DirectX::XMVectorScale(LightPosition, -250.0f);
			DirectX::XMMATRIX V = DirectX::XMMatrixLookAtLH(LightPosition,
				DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
				DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

			// シャドウマップに描画したい範囲の射影行列を生成
			DirectX::XMMATRIX P = DirectX::XMMatrixOrthographicLH(gfx.shadowDrawRect, gfx.shadowDrawRect, 0.1f, 1000.0f);
			XMMATRIX viewProjection = V * P;
			DirectX::XMStoreFloat4x4(&gfx.shadowMapData.lightViewProjection, viewProjection);	// ビュー　プロジェクション　変換行列をまとめる
		}


		gfx.deviceContext->UpdateSubresource(gfx.constantBuffers[5].Get(), 0, 0, &gfx.shadowMapData, 0, 0);
		gfx.deviceContext->VSSetConstantBuffers(3, 1, gfx.constantBuffers[5].GetAddressOf());

		gfx.deviceContext->VSSetShader(gfx.vertexShaders[static_cast<size_t>(VS_TYPE::ShadowMapCaster_VS)].Get(), nullptr, 0);
		gfx.deviceContext->PSSetShader(nullptr, nullptr, 0);

		StageManager::Instance().Render();

		SpinningTopPlayerManager::Instance().Render();

		SpinningTopEnemyManager::Instance().Render(true);

		ObstacleManager::Instance().Render();

		gfx.shadowBuffer->DeActivate();
	}

	gfx.bloomBuffer->Clear();

	gfx.bloomBuffer->Activate();

	skyMap->Render();

	gfx.SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);
	gfx.SetRasterizer(static_cast<RASTERIZER_STATE>(RASTERIZER_STATE::CLOCK_FALSE_SOLID));

	Graphics::SceneConstants data {};
	XMMATRIX viewProjection = XMLoadFloat4x4(&Camera::Instance().GetView()) * XMLoadFloat4x4(&Camera::Instance().GetProjection());
	DirectX::XMStoreFloat4x4(&data.viewProjection, viewProjection);	// ビュー　プロジェクション　変換行列をまとめる

	LightManager::Instance().PushLightData(data);

	data.cameraPosition = { Camera::Instance().GetEye().x, Camera::Instance().GetEye().y, Camera::Instance().GetEye().z, 0 };
	gfx.deviceContext->UpdateSubresource(gfx.constantBuffer.Get(), 0, 0, &data, 0, 0);
	gfx.deviceContext->VSSetConstantBuffers(1, 1, gfx.constantBuffer.GetAddressOf());
	gfx.deviceContext->PSSetConstantBuffers(1, 1, gfx.constantBuffer.GetAddressOf());

	// shadowMap のバインド
	gfx.deviceContext->PSSetShaderResources(4, 1, gfx.shadowBuffer->shaderResourceView.GetAddressOf());
	gfx.deviceContext->PSSetSamplers(3, 1, gfx.samplerStates[static_cast<size_t>(SAMPLER_STATE::SHADOWMAP)].GetAddressOf());

	gfx.deviceContext->VSSetShader(gfx.vertexShaders[static_cast<size_t>(VS_TYPE::SkinnedMesh_VS)].Get(), nullptr, 0);
	gfx.deviceContext->PSSetShader(gfx.pixelShaders[static_cast<size_t>(PS_TYPE::SkinnedMesh_PS)].Get(), nullptr, 0);

	gfx.deviceContext->UpdateSubresource(gfx.constantBuffers[5].Get(), 0, 0, &gfx.shadowMapData, 0, 0);
	gfx.deviceContext->VSSetConstantBuffers(3, 1, gfx.constantBuffers[5].GetAddressOf());
	gfx.deviceContext->PSSetConstantBuffers(3, 1, gfx.constantBuffers[5].GetAddressOf());

	StageManager::Instance().Render();

	SpinningTopEnemyManager::Instance().Render();

	ObstacleManager::Instance().Render();

	SpinningTopPlayerManager::Instance().Render();


	//gfx.SetDepthStencil(DEPTHSTENCIL_STATE::ZT_OFF_ZW_OFF);
	gfx.SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);

	DamageTextManager::Instance().Render();

	//DispString::Instance().Draw(L"HO感SHＨIN LiうB", { 800, 60 }, 32, TEXT_ALIGN::MIDDLE, { 0, 0, 0, 1 });

	// --- デバッグ描画 ---
	//DebugPrimitive::Instance().Render();
	//LineRenderer::Instance().Render();



	gfx.bloomBuffer->DeActivate();

	gfx.SetDepthStencil(DEPTHSTENCIL_STATE::ZT_OFF_ZW_OFF);

#if 1
	// --- 高輝度抽出 ---
	gfx.frameBuffers[1]->Clear();

	gfx.deviceContext->UpdateSubresource(gfx.constantBuffers[4].Get(), 0, 0, &gfx.luminanceExtractionConstant, 0, 0);
	gfx.deviceContext->PSSetConstantBuffers(0, 1, gfx.constantBuffers[4].GetAddressOf());
	gfx.frameBuffers[1]->Activate();
	gfx.bitBlockTransfer->blit(gfx.bloomBuffer->shaderResourceViews[0].GetAddressOf(), 0, 2, gfx.pixelShaders[static_cast<size_t>(PS_TYPE::LuminanceExtraction_PS)].Get());
	gfx.frameBuffers[1]->DeActivate();
	gfx.bitBlockTransfer->blit(gfx.frameBuffers[0]->shaderResourceViews[0].GetAddressOf(), 0, 1);

	// --- ガウシアンフィルタ ---
	gfx.CalcWeightsTableFromGaussian(gaussianPower);
	gfx.deviceContext->UpdateSubresource(gfx.constantBuffers[3].Get(), 0, 0, &gfx.gaussianConstant, 0, 0);
	gfx.deviceContext->PSSetConstantBuffers(0, 1, gfx.constantBuffers[3].GetAddressOf());

	gfx.frameBuffers[2]->Activate();
	gfx.bitBlockTransfer->blit(gfx.frameBuffers[1]->shaderResourceViews[0].GetAddressOf(), 0, 1, gfx.pixelShaders[static_cast<size_t>(PS_TYPE::GaussianBlur_PS)].Get(), gfx.vertexShaders[static_cast<size_t>(VS_TYPE::GaussianBlurX_VS)].Get());
	gfx.frameBuffers[2]->DeActivate();

	gfx.frameBuffers[3]->Activate();
	gfx.bitBlockTransfer->blit(gfx.frameBuffers[2]->shaderResourceViews[0].GetAddressOf(), 0, 1, gfx.pixelShaders[static_cast<size_t>(PS_TYPE::GaussianBlur_PS)].Get(), gfx.vertexShaders[static_cast<size_t>(VS_TYPE::GaussianBlurY_VS)].Get());
	gfx.frameBuffers[3]->DeActivate();

	// --- ブルーム加算 ---
	ID3D11ShaderResourceView* shvs[2] =
	{
		gfx.bloomBuffer->shaderResourceViews[0].Get(),
		gfx.frameBuffers[3]->shaderResourceViews[0].Get()
	};
	gfx.frameBuffers[4]->Activate();
	gfx.bitBlockTransfer->blit(shvs, 0, 2, gfx.pixelShaders[static_cast<size_t>(PS_TYPE::BloomFinalPass_PS)].Get());
	gfx.frameBuffers[4]->DeActivate();

	// --- カラーフィルター ---
	//gfx.deviceContext->UpdateSubresource(gfx.constantBuffers[2].Get(), 0, 0, &gfx.colorFilterConstant, 0, 0);
	//gfx.deviceContext->PSSetConstantBuffers(3, 1, gfx.constantBuffers[2].GetAddressOf());
	//
	//gfx.frameBuffers[5]->Activate();
	//gfx.bitBlockTransfer->blit(gfx.frameBuffers[4]->shaderResourceViews[0].GetAddressOf(), 0, 1, gfx.pixelShaders[static_cast<size_t>//(PS_TYPE::ColorFilter_PS)].Get());
	//gfx.frameBuffers[5]->DeActivate();


	gfx.bitBlockTransfer->blit(gfx.frameBuffers[4]->shaderResourceViews[0].GetAddressOf(), 0, 1);
#else
	gfx.bitBlockTransfer->blit(gfx.bloomBuffer->shaderResourceViews[0].GetAddressOf(), 0, 1);
#endif

	if (stateMap["DrawVideo"])videoUI.Draw(stateMap["StopUpdate"]);
	if (stateMap["TutorialStart"]) {
		rect.Render(0, 0,
			Framework::Instance().windowWidth, Framework::Instance().windowHeight,
			0, 0, 0, 0.5f, 0);

		sprite[1]->Render(
			sPos.x - sSize.x * .5f, sPos.y - sSize.y * .5f,
			sSize.x, sSize.y,
			1, 1, 1, 1, 0
		);

	}
	// --- デバッグ描画 ---
	DrawDebugGUI();
}

void SceneSTTutorial::DrawDebugGUI() {
	if (ImGui::Begin("Easing")) {
		ImGui::Text("time:%f", time);
		ImGui::Text("easing:%f", ease);
		if (ImGui::Button("reset")) {
			time = 0;
		}

		const char* Estr[] = {
			"easeSine",
			"easeQuad",
			"easeCubic",
			"easeQuart",
			"easeQuint",
			"easeExpo",
			"easeCirc",
			"easeBack",
			"easeElastic",
			"easeBounce",
			"easeEnd",
		};

		ImGui::ListBox("EasingFunc", &Efunc, Estr, 11);
		ImGui::End();
	}
}
