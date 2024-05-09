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
#include "Library/Audio/AudioManager.h"

#include "StageManager.h"
#include "LightManager.h"
#include "SpinningTopEnemyManager.h"
#include "StEnemy.h"
#include "SpinningTopPlayerManager.h"
#include "DataManager.h"
#include "ObstacleManager.h"
#include "ObsStaticObj.h"
#include "ObsMarunoko.h"
#include "DamageTextManager.h"
#include "Wave.h"

#include <random>
#include <string>

void SceneSTTutorial::Initialize() {
	VideoManager::Instance().LoadFile(PARRY, L"Data/Video/Parry.mp4");
	VideoManager::Instance().LoadFile(ATTACK, L"Data/Video/Attack.mp4");
	VideoManager::Instance().LoadFile(GAUGEATTACK, L"Data/Video/GaugeAttack.mp4");
	VideoManager::Instance().LoadFile(GETOPTION, L"Data/Video/Option.mp4");
	sprite.emplace_back(std::make_unique<Sprite>(L"Data/Texture/Tutorial/Enter.png"));
	sprite.emplace_back(std::make_unique<Sprite>(L"Data/Texture/Tutorial/Parry1.png"));
	sprite.emplace_back(std::make_unique<Sprite>(L"Data/Texture/Tutorial/GaugeParry.png"));
	sprite.emplace_back(std::make_unique<Sprite>(L"Data/Texture/Tutorial/BodyBlow.png"));
	sprite.emplace_back(std::make_unique<Sprite>(L"Data/Texture/Tutorial/Tutorial.png"));
	sprite.emplace_back(std::make_unique<Sprite>(L"Data/Texture/Tutorial/Clear.png"));
	sprite.emplace_back(std::make_unique<Sprite>(L"Data/Texture/Tutorial/BackToTutorial.png"));
	sprite.emplace_back(std::make_unique<Sprite>(L"Data/Texture/Tutorial/Parry2.png"));
	sprite.emplace_back(std::make_unique<Sprite>(L"Data/Texture/Tutorial/Parry.png"));
	sprite.emplace_back(std::make_unique<Sprite>(L"Data/Texture/Input/Input.png"));
	sprite.emplace_back(std::make_unique<Sprite>(L"Data/Texture/Input/InputController.png"));

	spriteControl.emplace_back(std::make_unique<Sprite>(L"Data/Texture/Tutorial/EnterControl.png"));
	spriteControl.emplace_back(std::make_unique<Sprite>(L"Data/Texture/Tutorial/Parry1Control.png"));
	spriteControl.emplace_back(std::make_unique<Sprite>(L"Data/Texture/Tutorial/GaugeParryControl.png"));
	spriteControl.emplace_back(std::make_unique<Sprite>(L"Data/Texture/Tutorial/BodyBlowControl.png"));
	spriteControl.emplace_back(std::make_unique<Sprite>(L"Data/Texture/Tutorial/BackToTutorialControl.png"));
	spriteControl.emplace_back(std::make_unique<Sprite>(L"Data/Texture/Tutorial/BackToTutorialControl.png"));
	spriteControl.emplace_back(std::make_unique<Sprite>(L"Data/Texture/Tutorial/BackToTutorialControl.png"));
	spriteControl.emplace_back(std::make_unique<Sprite>(L"Data/Texture/Tutorial/Parry2Control.png"));
	spriteControl.emplace_back(std::make_unique<Sprite>(L"Data/Texture/Tutorial/Parry.png"));

	spriteUI.emplace_back(std::make_unique<Sprite>(L"Data/Texture/HP0.png"));
	spriteUI.emplace_back(std::make_unique<Sprite>(L"Data/Texture/HP1.png"));
	spriteUI.emplace_back(std::make_unique<Sprite>(L"Data/Texture/HP2.png"));
	spriteUI.emplace_back(std::make_unique<Sprite>(L"Data/Texture/HP3.png"));
	spriteUI.emplace_back(std::make_unique<Sprite>(L"Data/Texture/rotSpeedBottom.png"));
	spriteUI.emplace_back(std::make_unique<Sprite>(L"Data/Texture/rotSpeedMiddle.png"));
	spriteUI.emplace_back(std::make_unique<Sprite>(L"Data/Texture/rotSpeedMiddleMask.png"));
	spriteUI.emplace_back(std::make_unique<Sprite>(L"Data/Texture/rotSpeedTop.png"));

	videoUI.SetVideo(&VideoManager::Instance().GetVideo(PARRY));
	videoUI.SetTextSprite((InputManager::Instance().IsGamePadConnected() ? spriteControl : sprite)[PARRY].get());
	VideoManager::Instance().Play(PARRY,true);
	stateMap["StopUpdate"] = true;
	stateMap["DrawVideo"] = false;
	stateMap["TutorialStart"] = true;

	DirectX::XMFLOAT2 texSize = sprite[START - 1]->GetTexSize();
	sSize = texSize;
	sPos = { -sSize.x * .5f,Framework::Instance().windowHeight * .5f };

	DataManager::Instance().LoadEnemyData(enemyData);
	DataManager::Instance().LoadSpawnEreaData();

	// プレイヤー初期化
	StPlayerBase* player = new StPlayer();
	player->SetRotateSpeed(1);
	SpinningTopPlayerManager::Instance().Register(player);


	// Cylinder
	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			ObsStaticObj* obstacle = new_ ObsStaticObj("Data/FBX/StGaitou/StGaitou.fbx");
			obstacle->SetScale({ 4, 1, 4 });
			obstacle->SetPosition({ i * 50.0f - 25.0f, 0, j * 50.0f - 25.0f });
			obstacle->SetRadius(0.4f);
			ObstacleManager::Instance().Register(obstacle);
		}
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
	LightManager::Instance().SetAmbientColor({ 0.2f, 0.2f, 0.2f, 1.0f });


	// スカイマップ
	//skyMap = std::make_unique<SkyMap>(L"Data/Texture/kloppenheim_05_puresky_4k.hdr");

	// カメラ初期設定
	Camera::Instance().SetLookAt(
		DirectX::XMFLOAT3(0, 90, 60),		// カメラ座標
		DirectX::XMFLOAT3(-90, 0, -30),		// ターゲット(設定しても意味ない)
		DirectX::XMFLOAT3(0, 1, 0)			// 上方向ベクトル
	);
	Camera::Instance().SetAngle({ DirectX::XMConvertToRadians(60), DirectX::XMConvertToRadians(180), 0 });

	Camera::Instance().cameraType = Camera::CAMERA::TargetStPlayer;

	pause = std::make_unique<Pause>();

	Wave::Instance().Init();

	AudioManager::Instance().LoadMusic(BGM_TRACK::BGM_1, L"Data/Audio/BGM/Game.wav");
	AudioManager::Instance().PlayMusic(BGM_TRACK::BGM_1, true);
	AudioManager::Instance().SetMusicVolume(BGM_TRACK::BGM_1, kMasterVolume);
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
	VideoManager::Instance().Update();
	pause->Update();
	if (pause->isPause) {
		videoUI.GetVideo()->Pause();
		return;
	}
	else {
		videoUI.GetVideo()->Play(true);
	}

	Camera::Instance().Update();

	if(!stateMap["StopUpdate"]) {
		// カメラコントローラー更新処理

		SpinningTopPlayerManager::Instance().Update();

		StageManager::Instance().Update();

		SpinningTopEnemyManager::Instance().Update();

		ObstacleManager::Instance().Update();

		DamageTextManager::Instance().Update();
	}
	UpdateState();
	UpdateTutorial();
}

void SceneSTTutorial::UpdateState() {
	constexpr float kHalfTime = .9f;
	InputManager& im = InputManager::Instance();
	float width = Framework::Instance().windowWidth;
	float height = Framework::Instance().windowHeight;
	float wRatio = width / 1280;
	float hRatio = height / 720;
	float sRatio = wRatio * hRatio;

	if (stateMap["TutorialStart"]) {

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
		//videoUI.SetSize(sSize);
		if (im.GetKeyPressed(Keyboard::Enter) || im.GetGamePadButtonPressed(GAMEPADBUTTON_STATE::a)) {
			if (tState != ATTACK) {
				stateMap["StopUpdate"] = false;
				stateMap["UpdateTarm"] = true;
				stateMap["DrawText"] = true;
				SpinningTopPlayerManager::Instance().GetPlayer(0)->SetPosition({ 0,0,0 });
			}
			else {
				videoUI.SetTextSprite((InputManager::Instance().IsGamePadConnected() ? spriteControl : sprite)[PARRY2 - 1].get());
				nextPage++;
				if (nextPage >= 2) {
					videoUI.SetTextSprite((InputManager::Instance().IsGamePadConnected() ? spriteControl : sprite)[PARRYALWAYS - 1].get());
					stateMap["StopUpdate"] = false;
					stateMap["UpdateTarm"] = true;
					stateMap["DrawText"] = true;
					SpinningTopPlayerManager::Instance().GetPlayer(0)->SetPosition({ 0,0,0 });
				}
			}
		}
	}
	else {
		videoUI.SetPosition({ 1060 * wRatio,144 * hRatio });
		videoUI.SetSize({ 425 * wRatio,281 * hRatio });
	}

	if (tarm == 0 && !stateMap["TutorialClear"]) {
		tarm = -1;
		stateMap["TutorialClear"] = true;
		stateMap["DrawText"] = false;
	}

	if (stateMap["TutorialClear"]) {
		time += Timer::Instance().DeltaTime();
		if (time < kHalfTime)ease = (sSize.x + width) * 0.5f * Easing(time, kHalfTime, easeCubic, easeOut);
		else ease = (sSize.x + width) * 0.5f * (1 + Easing(time - kHalfTime, kHalfTime, easeCubic, easeIn));
		sPos.x = -sSize.x * .5f + ease;
		if (time > kHalfTime * 2) {
			stateMap["TutorialClear"] = false;
			tState++;
			if (tState == SPSTART) {
				stateMap["TutorialEnd"] = true;
			}
			else {
				stateMap["StopUpdate"] = true;

				videoUI.SetVideo(&VideoManager::Instance().GetVideo(tState));
				videoUI.SetTextSprite((InputManager::Instance().IsGamePadConnected() ? spriteControl : sprite)[tState].get());
				VideoManager::Instance().Play(tState, true);
				VideoManager::Instance().Stop(tState - 1);
			}
			time = 0;
		}
	}

	if (SpinningTopPlayerManager::Instance().GetPlayer(0)->isDead) {
		stateMap["DeadPlayer"] = true;
	}

	if (stateMap["DeadPlayer"]) {
		spawnTime += Timer::Instance().DeltaTime();
		if (spawnTime > 2) {
			SpinningTopPlayerManager::Instance().Clear();
			StPlayer* player = new StPlayer();
			player->SetPosition({ 0,0,0 });
			SpinningTopPlayerManager::Instance().Register(player);
			stateMap["DeadPlayer"] = false;
			spawnTime = 0;
		}
	}

}

void SceneSTTutorial::UpdateTutorial() {
	auto enemySpawn = [](float a, float b) {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<float> dis(a, b);

		StEnemy* enemy = new StEnemy(0);

		float xPos = SpinningTopPlayerManager::Instance().GetPlayer(0)->GetPosition().x;
		float zPos = SpinningTopPlayerManager::Instance().GetPlayer(0)->GetPosition().x;

		float rad = DirectX::XMConvertToRadians(rand() % 360);
		float dist = dis(gen);

		xPos += cosf(rad) * dist;
		zPos += sinf(rad) * dist;

		enemy->SetPosition({ xPos, 10, zPos });
		SpinningTopEnemyManager::Instance().Register(enemy);

	};

	switch (tState) {
	case SceneSTTutorial::PARRY:
		if (stateMap["UpdateTarm"]) {
			tarm = 5;
			text = 5;
		}
		if (StPlayerBase::GetInputMap<bool>("Attack"))
			tarm--;
		StPlayerBase::SetRotateSpeed(1);
		break;

	case SceneSTTutorial::ATTACK:
		if (stateMap["UpdateTarm"]) {
			for (size_t i = 0; i < 4; i++){
				enemySpawn(4, 10);
				tarm = SpinningTopEnemyManager::Instance().GetEnemyCount();
				text = 4;
			}
		}
		if (tarm > 0)
			tarm = SpinningTopEnemyManager::Instance().GetEnemyCount();

		StPlayerBase::SetRotateSpeed(1);
		break;

	case SceneSTTutorial::GAUGEATTACK:
		if (stateMap["UpdateTarm"]) {
			enemySpawn(10, 15);
			tarm = SpinningTopEnemyManager::Instance().GetEnemyCount();
			text = 1;
		}
		if (tarm > 0)
			tarm = SpinningTopEnemyManager::Instance().GetEnemyCount();
		try{
			auto* player = SpinningTopPlayerManager::Instance().GetPlayer(0);
			player->SetRotateSpeed(player->GetData()->rotateMaxSpeed);
		}
		catch(std::out_of_range&){}
		break;

	case SceneSTTutorial::GETOPTION:
		if (stateMap["UpdateTarm"]) {
			tarm = 5;
			text = 5;
		}
		if (SpinningTopEnemyManager::Instance().GetEnemyCount() == 0) {
			enemySpawn(4, 5);
			tarm--;
		}
		try{
			auto* player = SpinningTopPlayerManager::Instance().GetPlayer(0);
			player->SetRotateSpeed(player->GetData()->rotateMaxSpeed);
		}
		catch (std::out_of_range&) {}
		break;

	default:
		if (SpinningTopEnemyManager::Instance().GetEnemyCount() == 0) {
			for (size_t i = 0; i < 4; i++) {
				enemySpawn(5, 10);
				tarm = SpinningTopEnemyManager::Instance().GetEnemyCount();
				text = 4;
			}
		}
	}
	stateMap["UpdateTarm"] = false;
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

		if (!stateMap["DeadPlayer"])SpinningTopPlayerManager::Instance().Render();

		SpinningTopEnemyManager::Instance().Render(true);

		ObstacleManager::Instance().Render();

		gfx.shadowBuffer->DeActivate();
	}

	gfx.bloomBuffer->Clear();

	gfx.bloomBuffer->Activate();

	//skyMap->Render();

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

	if (!stateMap["DeadPlayer"])SpinningTopPlayerManager::Instance().Render();


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

	float width = Framework::Instance().windowWidth;
	float height = Framework::Instance().windowHeight;
	float wRatio = width / 1280;
	float hRatio = height / 720;
	float sRatio = wRatio * hRatio;
	float hpBairitu = 0.75;

	if (SpinningTopPlayerManager::Instance().GetPlayerCount() > 0) {
		int hp = SpinningTopPlayerManager::Instance().GetPlayer(0)->GetHealth();
		switch (hp) {
		case 0: spriteUI[0]->Render(25, 25, 256 * 0.666 * hpBairitu, 256 * 0.666 * hpBairitu, 1, 1, 1, 1, 0); break;
		case 1: spriteUI[1]->Render(25, 25, 256 * 0.666 * hpBairitu, 256 * 0.666 * hpBairitu, 1, 1, 1, 1, 0); break;
		case 2: spriteUI[2]->Render(25, 25, 256 * 0.666 * hpBairitu, 256 * 0.666 * hpBairitu, 1, 1, 1, 1, 0); break;
		case 3: spriteUI[3]->Render(25, 25, 256 * 0.666 * hpBairitu, 256 * 0.666 * hpBairitu, 1, 1, 1, 1, 0); break;
		}

		StPlayerBase* player = SpinningTopPlayerManager::Instance().GetPlayer(0);
		PlayerData* data = player->GetData();
		float maxRotSpeed = data->rotateMaxSpeed;
		float rotSpeed = player->GetRotationSpeed();

		float hAspect = rotSpeed / maxRotSpeed;

		//hAspect = 0.5f;
		float height = (320 - 26) * (1.0f - hAspect);

		DirectX::XMFLOAT2 rotUiPos = { 30, 200 };

		float rotSpeedBairitu = 0.75f;

		spriteUI[4]->Render(rotUiPos.x, rotUiPos.y, 56 * rotSpeedBairitu, 320 * rotSpeedBairitu, 1, 1, 1, 1, 0);
		spriteUI[5]->Render(rotUiPos.x, rotUiPos.y + 13 * rotSpeedBairitu, 56 * rotSpeedBairitu, 294 * rotSpeedBairitu, 1, 1, 1, 1, 0);
		spriteUI[6]->Render(rotUiPos.x, rotUiPos.y + 13 * rotSpeedBairitu, 56 * rotSpeedBairitu, height * rotSpeedBairitu, 1, 1, 1, 1, 0, 0, 0, 56, (320 - 26) * (1.0f - hAspect));
		spriteUI[7]->Render(rotUiPos.x, rotUiPos.y, 56 * rotSpeedBairitu, 320 * rotSpeedBairitu, 1, 1, 1, 1, 0);
	}

	if (stateMap["DrawVideo"]) {
		DirectX::XMFLOAT2 cPos = { 1170 * wRatio,650 * hRatio };
		DirectX::XMFLOAT2 cSize = { 160 * wRatio,99 * hRatio };
		sprite[(InputManager::Instance().IsGamePadConnected() ? CONTROLLER : KEYBORD) - 1]->Render(
			cPos.x - cSize.x * .5f, cPos.y - cSize.y * .5f,
			cSize.x, cSize.y,
			1, 1, 1, 1, 0);
		videoUI.Draw({ 0.25f,0.640625f,0.875f,1 }, stateMap["StopUpdate"]);
	}

	if (stateMap["TutorialStart"]) {
		rect.Render(0, 0,
			width, height,
			0, 0, 0, 0.5f, 0);

		sprite[START - 1]->Render(
			sPos.x - sSize.x * .5f, sPos.y - sSize.y * .5f,
			sSize.x, sSize.y,
			1, 1, 1, 1, 0
		);

	}
	if (stateMap["TutorialClear"]) {
		sprite[CLEAR - 1]->Render(
			sPos.x - sSize.x * .5f, sPos.y - sSize.y * .5f,
			sSize.x, sSize.y,
			1, 1, 1, 1, 0
		);
	}
	if (stateMap["DrawText"]) {
		std::wstring str = std::to_wstring(text - tarm) + L"/" + std::to_wstring(text);
		DispString::Instance().Draw(str.c_str(), { width * .5f ,0 }, 32, TEXT_ALIGN::UPPER_MIDDLE, { 1,1,1,1 });
	}
	if (stateMap["TutorialEnd"]) {
		DirectX::XMFLOAT2 cPos = { 640 * wRatio,535 * hRatio };
		DirectX::XMFLOAT2 cSize = { 488 * wRatio,91.2f * hRatio };
		(InputManager::Instance().IsGamePadConnected() ? spriteControl : sprite)[BACKTOTUTORIAL - 1]->Render(
			cPos.x - cSize.x * .5f, cPos.y - cSize.y * .5f,
			cSize.x, cSize.y,
			1, 1, 1, 1, 0
		);
	}
	pause->Render();
	// --- デバッグ描画 ---
	//DrawDebugGUI();
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

		ImGui::Text("Tarm:%d", tarm);
		if (ImGui::Button("tarmClear")) {
			tarm = 0;
		}

		if (tState == ATTACK) {
			if (ImGui::Button("EnemyCrear")) {
				SpinningTopEnemyManager::Instance().Clear();
			}
		}

		ImGui::DragFloat2("size", &dSize.x);
		ImGui::DragFloat2("pos", &dPos.x);
		XMFLOAT3 playerPos;
		try {
			playerPos = SpinningTopPlayerManager::Instance().GetPlayer(0)->GetPosition();

		}
		catch (const std::out_of_range&) {
			playerPos = {};
		}
		ImGui::DragFloat3("playerPos", &playerPos.x);
		ImGui::End();
	}
}
