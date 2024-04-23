#include <tchar.h>
#include "SceneSpinningTop.h"
#include "SceneManager.h"
#include "Library/Input/InputManager.h"
#include "Library/ImGui/Include/imgui.h"
#include "Library/Timer.h"
#include "StageManager.h"
#include "StageContext.h"
#include "LightManager.h"
#include "SpinningTopEnemyManager.h"
#include "ObstacleManager.h"
#include "StEnemy01.h"
#include "StEnemy02.h"
#include "StEnemy.h"
#include "Library/3D/DebugPrimitive.h"
#include "Library/3D/LineRenderer.h"

#include "DataManager.h"

void SceneSpinningTop::Initialize()
{
	DataManager::Instance().LoadEnemyData(enemyData);

	for (int i = 0; i < 4; i++)
	{
		StEnemy* enemy;
		enemy = (i % 2 == 0) ? new_ StEnemy(ENEMY_0) : new_ StEnemy(ENEMY_1);
		enemy->SetPosition({ i * 3.0f - 10.5f, 10, 0 });
		// スポーン座標設定
		enemy->spawnPosition = enemy->GetPosition();
		SpinningTopEnemyManager::Instance().Register(enemy);
	}
	
	for (int i = 0; i < 3; i++)
	{
		Obstacle* obstacle = new_ Obstacle("Data/FBX/cylinder/cylinder.fbx");
		obstacle->SetPosition({ i * 10.0f - 15.0f, 0, -6.0f });
		ObstacleManager::Instance().Register(obstacle);
	}
	


	// ステージ初期化
	StageManager& stageManager = StageManager::Instance();
	StageContext* stageMain = new_ StageContext();
	stageManager.Register(stageMain);

#if 1
	// ライト初期化
	Light* directionLight = new Light(LightType::Directional);
	directionLight->SetDirection(DirectX::XMFLOAT3(0.5, -1, -1));
	directionLight->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
	LightManager::Instance().Register(directionLight);

#else
	{
		// ライト初期化
		Light* directionLight = new Light(LightType::Directional);
		directionLight->SetDirection(DirectX::XMFLOAT3(0.5, -1, -1));
		//directionLight->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
		directionLight->SetColor(DirectX::XMFLOAT4(0, 0, 0, 1));
		LightManager::Instance().Register(directionLight);

		// 点光源追加
		{
			Light* light = new Light(LightType::Point);
			light->SetPosition(DirectX::XMFLOAT3(2, 1, 0));
			light->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
			light->SetRange(30.0f);
			LightManager::Instance().Register(light);

			//Light* light2 = new Light(LightType::Point);
			//light2->SetPosition(DirectX::XMFLOAT3(-2, 1, 0));
			//light2->SetColor(DirectX::XMFLOAT4(1, 0, 1, 1));
			//LightManager::Instance().Register(light2);
		}
		// スポットライトを追加
		{
			Light* light = new Light(LightType::Spot);
			light->SetPosition(DirectX::XMFLOAT3(-3, 2, 0));
			light->SetColor(DirectX::XMFLOAT4(1, 0, 0, 1));
			light->SetDirection(DirectX::XMFLOAT3(+1, -1, 0));
			light->SetRange(4.0f);
			LightManager::Instance().Register(light);
		}

		LightManager::Instance().SetAmbientColor({ 0,0,0,1.0f });
#endif

	// スカイマップ
	skyMap = std::make_unique<SkyMap>(L"Data/Texture/kloppenheim_05_puresky_4k.hdr");


	// カメラ初期設定
	Camera::Instance().SetLookAt(
		DirectX::XMFLOAT3(0, 90, 60),		// カメラ座標
		DirectX::XMFLOAT3(-90, 0, -30),		// ターゲット(設定しても意味ない)
		DirectX::XMFLOAT3(0, 1, 0)			// 上方向ベクトル
	);
	Camera::Instance().SetAngle({ DirectX::XMConvertToRadians(60), DirectX::XMConvertToRadians(180), 0 });

	Camera::Instance().cameraType = Camera::CAMERA::FREE;
}

void SceneSpinningTop::Finalize()
{
	StageManager::Instance().Clear();

	LightManager::Instance().Clear();

	SpinningTopEnemyManager::Instance().Clear();

	ObstacleManager::Instance().Clear();
}

void SceneSpinningTop::Update()
{
	// カメラコントローラー更新処理
	Camera::Instance().Update();

	StageManager::Instance().Update();

	SpinningTopEnemyManager::Instance().Update();

	ObstacleManager::Instance().Update();
}

void SceneSpinningTop::Render()
{
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

		SpinningTopEnemyManager::Instance().Render(true);

		ObstacleManager::Instance().Render();

		gfx.shadowBuffer->DeActivate();
	}

	skyMap->Render();


	gfx.SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);
	gfx.SetRasterizer(static_cast<RASTERIZER_STATE>(RASTERIZER_STATE::CLOCK_FALSE_SOLID));

	Graphics::SceneConstants data{};
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

	// --- デバッグ描画 ---
	DebugPrimitive::Instance().Render();
	LineRenderer::Instance().Render();

	// --- デバッグ描画 ---
	DrawDebugGUI();

}

// デバッグ描画
void SceneSpinningTop::DrawDebugGUI()
{
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::BeginMenu("Scene")) {
				if (ImGui::MenuItem("Title"))
				{
					SceneManager::Instance().ChangeScene(new SceneTitle);
				}
				if (ImGui::MenuItem("Game"))
				{
					SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
				}
				if (ImGui::MenuItem("Animation"))
				{
					SceneManager::Instance().ChangeScene(new SceneAnimation);
				}
				if (ImGui::MenuItem("ContextBase"))
				{
					SceneManager::Instance().ChangeScene(new SceneLoading(new SceneContextBase));
				}
				if (ImGui::MenuItem("Test"))
				{
					SceneManager::Instance().ChangeScene(new SceneLoading(new SceneTest));
				}
				if (ImGui::MenuItem("SpinningTop"))
				{
					SceneManager::Instance().ChangeScene(new SceneLoading(new SceneSpinningTop));
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	SpinningTopEnemyManager::Instance().DrawDebugGui();
	ObstacleManager::Instance().DrawDebugGui();
}
