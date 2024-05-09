#include "SceneSTMainMenu.h"
#include "SceneManager.h"
#include "Library/Input/InputManager.h"
#include "Library/ImGui/Include/imgui.h"
#include "Library/Text/DispString.h"
#include "Library/Text/Text.h"
#include "Library/Timer.h"
#include "Library/Input/InputManager.h"
#include "Library/Framework.h"
#include "Library/Easing.h"
#include "Library/Audio/AudioManager.h"

#include "Video.h"
#include "StageManager.h"
#include "LightManager.h"
#include "StMenuPlayer.h"

#include <tchar.h>
#include <filesystem>

enum SPRITENAME {
	WAVE = 0,
	TUTORIAL,
	BACK,
	MAINMANU,
};

void SceneSTMainMenu::Initialize() {
	player.SetPosition({ 14,0,0 });
	S3DObject.emplace_back(std::make_shared<Sprite3DObject>(L"Data/Texture/MainMenu/Wave.png"));
	S3DObject.emplace_back(std::make_shared<Sprite3DObject>(L"Data/Texture/MainMenu/Tutorial.png"));
	S3DObject.emplace_back(std::make_shared<Sprite3DObject>(L"Data/Texture/MainMenu/Back.png"));
	S3DObject.emplace_back(std::make_shared<Sprite3DObject>(L"Data/Texture/MainMenu/MainMenu.png"));

	S3DObject[WAVE]->SetPosition({ -7,.05f,0 });
	S3DObject[WAVE]->SetScaleInAsp(11);
	S3DObject[TUTORIAL]->SetPosition({ 7,.05f,0 });
	S3DObject[TUTORIAL]->SetScaleInAsp(11);
	S3DObject[BACK]->SetPosition({ 12,.05f,8.5f });
	S3DObject[BACK]->SetAngle({ DirectX::XM_PIDIV2,2.88f,0 });
	S3DObject[BACK]->SetScaleInAsp(6);
	S3DObject[MAINMANU]->SetPosition({ 13,.05f,-10.5f });
	S3DObject[MAINMANU]->SetScaleInAsp(11);

	sprite.emplace_back(std::make_unique<Sprite>(L"Data/Texture/Input/Input.png"));
	sprite.emplace_back(std::make_unique<Sprite>(L"Data/Texture/Input/InputController.png"));
	sprite.emplace_back(std::make_unique<Sprite>(L"Data/Texture/MainMenu/image0.png"));

	StageManager& stageManager = StageManager::Instance();
	stageMain = std::make_unique<StageContext>();
	stageManager.Register(stageMain.get());

	// ライト初期化
	Light* directionLight = new Light(LightType::Directional);
	directionLight->SetDirection(DirectX::XMFLOAT3(0.5, -1, -1));
	directionLight->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
	LightManager::Instance().Register(directionLight);

	// カメラ初期設定
	Camera::Instance().SetLookAt(
		DirectX::XMFLOAT3(0, 30, 25),		// カメラ座標
		DirectX::XMFLOAT3(0, 0, 0),		// ターゲット(設定しても意味ない)
		DirectX::XMFLOAT3(0, 1, 0)			// 上方向ベクトル
	);
	Camera::Instance().SetAngle({ DirectX::XMConvertToRadians(50), DirectX::XMConvertToRadians(180), 0 });
	Camera::Instance().cameraType = Camera::CAMERA::LOCK;

	AudioManager::Instance().LoadMusic(BGM_TRACK::BGM_1, L"Data/Audio/BGM/Main.wav");
	AudioManager::Instance().PlayMusic(BGM_TRACK::BGM_1, true);
	AudioManager::Instance().SetMusicVolume(BGM_TRACK::BGM_1, kMasterVolume);
}

void SceneSTMainMenu::Finalize() {
	StageManager::Instance().Clear();

	LightManager::Instance().Clear();
	AudioManager::Instance().UnLoadMusic(BGM_TRACK::BGM_1);
}

void SceneSTMainMenu::Update() {
	InputManager& im = InputManager::Instance();

	Camera::Instance().Update();
	player.Update();
	DirectX::XMFLOAT4 defaultColor = { 0,0,0,0 };
	DirectX::XMFLOAT4 selectColor = { 0,0,-.8f,0 };
	selectSound = false;

	for (auto& obj : S3DObject)
		obj->SetColor(defaultColor);

	if (S3DObject[WAVE]->CircleHitToPoint(player.GetPosition())) {
		selectSound = true;
		S3DObject[WAVE]->SetColor(selectColor);
		if (im.GetKeyPressed(Keyboard::Enter) || im.GetGamePadButtonPressed(GAMEPADBUTTON_STATE::a)) {
			SceneManager::Instance().ChangeScene(new SceneLoading(new SceneWave));
			AudioManager::Instance().PlayMusic(MAIN_CONFIRM);
			AudioManager::Instance().SetMusicVolume(MAIN_CONFIRM, kMasterVolume * kSEVolume);
		}
	}

	if (S3DObject[TUTORIAL]->CircleHitToPoint(player.GetPosition())) {
		selectSound = true;
		S3DObject[TUTORIAL]->SetColor(selectColor);
		if (im.GetKeyPressed(Keyboard::Enter) || im.GetGamePadButtonPressed(GAMEPADBUTTON_STATE::a)) {
			SceneManager::Instance().ChangeScene(new SceneLoading(new SceneSTTutorial));
			AudioManager::Instance().PlayMusic(MAIN_CONFIRM);
			AudioManager::Instance().SetMusicVolume(MAIN_CONFIRM, kMasterVolume * kSEVolume);
		}
	}

	if (S3DObject[BACK]->RectHitToPoint(player.GetPosition())) {
		selectSound = true;
		S3DObject[BACK]->SetColor(selectColor);
		if (im.GetKeyPressed(Keyboard::Enter) || im.GetGamePadButtonPressed(GAMEPADBUTTON_STATE::a)) {
			SceneManager::Instance().ChangeScene(new SceneLoading(new SceneSTTitle));
			AudioManager::Instance().PlayMusic(MAIN_CONFIRM);
			AudioManager::Instance().SetMusicVolume(MAIN_CONFIRM, kMasterVolume * kSEVolume);
		}
	}

#if 1
	if (S3DObject[MAINMANU]->RectHitToPoint(player.GetPosition())) {
		if (im.GetKeyPress(Keyboard::Enter) || im.GetGamePadButtonPress(GAMEPADBUTTON_STATE::a)) {
			S3DObject[MAINMANU]->SetColor(selectColor);
		}
		if (im.GetKeyPressed(Keyboard::Enter) || im.GetGamePadButtonPressed(GAMEPADBUTTON_STATE::a)) {
			AudioManager::Instance().PlayMusic(MAIN_CONFIRM);
			AudioManager::Instance().SetMusicVolume(MAIN_CONFIRM, kMasterVolume * kSEVolume);
			EasterEggCount++;
		}
	}

	if (EasterEggCount > 51) {
		EasterEggCount = 0;
		AudioManager::Instance().PlayMusic(MAIN_SE);
		AudioManager::Instance().SetMusicVolume(MAIN_SE, kMasterVolume * kSEVolume + 17);
		EasterEgg = true;
	}

	if (EasterEgg) {
		EasterEggTime += Timer::Instance().DeltaTime();
		if (EasterEggTime < 1)EasterEggPosX = 402.5f * Easing(EasterEggTime, 1, easeQuad, easeOut);
		else {
			EasterEggPosX = 402.5f * (1 - Easing(EasterEggTime - 1, 1, easeQuad, easeIn));
			if (EasterEggPosX == 0) {
				EasterEgg = false;
				EasterEggTime = 0;
			}
		}
	}
#endif
	static bool sound = false;
	if ((selectSound ^ sound) && selectSound) {
		AudioManager::Instance().PlayMusic(MAIN_SELECT);
		AudioManager::Instance().SetMusicVolume(MAIN_SELECT, kMasterVolume * kSEVolume);
	}
	sound = selectSound;

	XMFLOAT3 position = player.GetPosition();
	if (!isPlayerMove && (position.x != 14 || position.z != 0))
		isPlayerMove = true;
}

void SceneSTMainMenu::Render() {
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
		player.Render();

		gfx.shadowBuffer->DeActivate();
	}


	// --- ImGuiのデバッグ適用処理 ---
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
	player.Render();
	for (auto& obj:S3DObject) obj->Draw();

	float width = Framework::Instance().windowWidth;
	float height = Framework::Instance().windowHeight;
	float wRatio = width / 1280;
	float hRatio = height / 720;
	float sRatio = wRatio * hRatio;
	if (!isPlayerMove) {
		mask.Render(0, 0, width, height, 0, 0, 0, 0.8f, 0, 30 * sRatio, { wRatio * -485 ,0 });
	}

	DirectX::XMFLOAT2 cPos = { 1170 * wRatio,650 * hRatio };
	DirectX::XMFLOAT2 cSize = { 160 * wRatio,99 * hRatio };
	sprite[InputManager::Instance().IsGamePadConnected() ? 1 : 0]->Render(
		cPos.x - cSize.x * .5f, cPos.y - cSize.y * .5f,
		cSize.x, cSize.y,
		1, 1, 1, 1, 0);

	sprite[2]->Render(
		(-402.5f + EasterEggPosX) * wRatio, 0,
		402.5f * wRatio, 722.5f * hRatio,
		1, 1, 1, 1, 0
	);

	// --- デバッグ描画 ---
	DrawDebugGUI();
}

void SceneSTMainMenu::DrawDebugGUI() {
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::BeginMenu("Scene")) {
				if (ImGui::MenuItem("Title")) {
					SceneManager::Instance().ChangeScene(new SceneTitle);
				}
				if (ImGui::MenuItem("Game")) {
					SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
				}
				if (ImGui::MenuItem("Animation")) {
					SceneManager::Instance().ChangeScene(new SceneAnimation);
				}
				if (ImGui::MenuItem("ContextBase")) {
					SceneManager::Instance().ChangeScene(new SceneLoading(new SceneContextBase));
				}
				if (ImGui::MenuItem("Test")) {
					SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTest));
				}
				if (ImGui::MenuItem("SpinningTop")) {
					SceneManager::Instance().ChangeScene(new SceneLoading(new SceneSpinningTop));
				}
				if (ImGui::MenuItem("STPlayer")) {
					SceneManager::Instance().ChangeScene(new SceneLoading(new SceneSTPlayer));
				}
				if (ImGui::MenuItem("EnemyLevelEditor")) {
					SceneManager::Instance().ChangeScene(new SceneLoading(new SceneEnemyLevelEditor));
				}
				if (ImGui::MenuItem("SpawnEditor")) {
					SceneManager::Instance().ChangeScene(new SceneLoading(new SceneSpawnEditor));
				}
				if (ImGui::MenuItem("SceneWave")) {
					SceneManager::Instance().ChangeScene(new SceneLoading(new SceneWave));
				}
				if (ImGui::MenuItem("SceneSTTitle")) {
					SceneManager::Instance().ChangeScene(new SceneLoading(new SceneSTTitle));
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	if (ImGui::Begin("Sprite3D")) {
		//static DirectX::XMFLOAT3 defPos = S3DObject[0]->GetPosition();
		//static DirectX::XMFLOAT3 defAng = S3DObject[0]->GetAngle();
		//static DirectX::XMFLOAT3 defScl = S3DObject[0]->GetScale();
		//ImGui::ColorEdit4("color", &selectColor.x);
		int i = 0;
		for (auto& obj : S3DObject) {
			ImGui::Text(std::to_string(i).c_str());
			ImGui::DragFloat3((std::string("Position:") + std::to_string(i)).c_str(), &obj->GetPosition().x, 0.01f);
			ImGui::DragFloat3((std::string("Angle:") + std::to_string(i)).c_str(), &obj->GetAngle().x, DirectX::XM_PI / 180);

			ImGui::DragFloat((std::string("Scale:") + std::to_string(i)).c_str(), &obj->GetScaleAsp());
			obj->SetScaleInAsp(obj->GetScaleAsp());
			i++;
		}

		//if (ImGui::Button("Reset")) {
		//	S3DObject[0]->SetPosition(defPos);
		//	S3DObject[0]->SetAngle(defAng);
		//	S3DObject[0]->SetScale(defScl);
		//}

		ImGui::End();
	}

	if (ImGui::Begin("Camera")) {
		DirectX::XMFLOAT3 eye = Camera::Instance().GetEye();
		DirectX::XMFLOAT3 focus = Camera::Instance().GetFocus();


		ImGui::DragFloat3("Position", &eye.x);
		ImGui::DragFloat3("Focus", &focus.x);

		ImGui::End();
	}

	if (ImGui::Begin("Player")) {
		DirectX::XMFLOAT3 pos = player.GetPosition();
		ImGui::DragFloat3("Position", &pos.x);

		ImGui::End();
	}
}
