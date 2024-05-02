#include "SceneSTMainMenu.h"
#include "SceneManager.h"
#include "Library/Input/InputManager.h"
#include "Library/ImGui/Include/imgui.h"
#include "Library/Text/DispString.h"
#include "Library/Text/Text.h"
#include "Library/Timer.h"
#include "Library/Input/InputManager.h"

#include "Video.h"
#include "StageManager.h"
#include "LightManager.h"

#include <tchar.h>
#include <filesystem>

void SceneSTMainMenu::Initialize() {
	tutorialS3D = std::make_unique<Sprite3DObject>(L"Data/Texture/tyu-.png", DirectX::XMFLOAT2( 512,256 ));

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
		DirectX::XMFLOAT3(0, 20, 40),		// カメラ座標
		DirectX::XMFLOAT3(-90, 0, -30),		// ターゲット(設定しても意味ない)
		DirectX::XMFLOAT3(0, 1, 0)			// 上方向ベクトル
	);
	Camera::Instance().SetAngle({ DirectX::XMConvertToRadians(30), DirectX::XMConvertToRadians(180), 0 });
	Camera::Instance().cameraType = Camera::CAMERA::FREE;
}

void SceneSTMainMenu::Finalize() {
	StageManager::Instance().Clear();

	LightManager::Instance().Clear();
}

void SceneSTMainMenu::Update() {
	Camera::Instance().Update();
	player.Update();

	tutorialS3D->SetColor({ (float)!tutorialS3D->HitToPoint(player.GetPosition()), 1, 1, 1 });
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
	tutorialS3D->Draw();

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
		static DirectX::XMFLOAT3 defPos = tutorialS3D->GetPosition();
		static DirectX::XMFLOAT3 defAng = tutorialS3D->GetAngle();
		static DirectX::XMFLOAT3 defScl = tutorialS3D->GetScale();

		ImGui::DragFloat3("Position", &tutorialS3D->GetPosition().x, 0.01f);
		ImGui::DragFloat3("Angle", &tutorialS3D->GetAngle().x, DirectX::XM_PI / 180);

		static float scale = 1;
		ImGui::DragFloat("Scale", &scale);
		tutorialS3D->SetScaleInAsp(scale);

		if (ImGui::Button("Reset")) {
			tutorialS3D->SetPosition(defPos);
			tutorialS3D->SetAngle(defAng);
			tutorialS3D->SetScale(defScl);
		}

		ImGui::End();
	}
}
