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

	float x = 0;
	float z = 0;
	for (int i = 0; i < 1; i++)
	{
		StEnemy* enemy;
		enemy = (i % 2 == 0) ? new_ StEnemy(ENEMY_1) : new_ StEnemy(ENEMY_0);
		enemy->SetPosition({ x, 10, z });
		// スポーン座標設定
		enemy->spawnPosition = enemy->GetPosition();
		SpinningTopEnemyManager::Instance().Register(enemy);
		x += 3;
		if (i % 7 == 0)
		{
			z -= 3;
			x = 0;
		}
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

#if 0
	// ライト初期化
	Light* directionLight = new Light(LightType::Directional);
	directionLight->SetDirection(DirectX::XMFLOAT3(0.5, -1, -1));
	directionLight->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
	//directionLight->SetColor(DirectX::XMFLOAT4(0, 0, 0, 1));
	LightManager::Instance().Register(directionLight);
	//LightManager::Instance().SetAmbientColor({0,0,0,1});
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
	}
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

	gfx.bloomBuffer->Clear();

	gfx.bloomBuffer->Activate();

	skyMap->Render();

	gfx.SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);
	gfx.SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_SOLID);

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




	ObstacleManager::Instance().Render();

	SpinningTopEnemyManager::Instance().Render();
	// --- デバッグ描画 ---
	DebugPrimitive::Instance().Render();
	LineRenderer::Instance().Render();


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

	Graphics* gfx = &Graphics::Instance();
	ImGui::Begin("ColorFilter");
	{
		ImGui::SliderFloat("threshould", &gfx->luminanceExtractionConstant.threshould, 0.0f, 1.0f);
		ImGui::SliderFloat("intensity", &gfx->luminanceExtractionConstant.intensity, 0.0f, 10.0f);
		ImGui::SliderFloat("HueShift", &gfx->colorFilterConstant.hueShift, 0.0f, 360.0f);
		ImGui::SliderFloat("saturation", &gfx->colorFilterConstant.saturation, 0.0f, 2.0f);
		ImGui::SliderFloat("brightness", &gfx->colorFilterConstant.brightness, 0.0f, 2.0f);

		ImGui::DragFloat("gaussianPower", &gaussianPower, 0.1f, 0.1f, 16.0f);

		ImGui::SliderFloat("DrawRect", &gfx->shadowDrawRect, 1.0f, 2048.0f);
		ImGui::ColorEdit3("Color", &gfx->shadowMapData.shadowColor.x);
		ImGui::SliderFloat("Bias", &gfx->shadowMapData.shadowBias, 0.0f, 0.1f);


		ImGui::Image(gfx->shadowBuffer->shaderResourceView.Get(), { 256, 256 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
		ImGui::Image(gfx->bloomBuffer->shaderResourceViews[0].Get(), { 256, 144 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
		ImGui::Image(gfx->bloomBuffer->shaderResourceViews[1].Get(), { 256, 144 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
		ImGui::Image(gfx->frameBuffers[1]->shaderResourceViews[0].Get(), { 256, 144 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
		ImGui::Image(gfx->frameBuffers[2]->shaderResourceViews[0].Get(), { 256, 144 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
		ImGui::Image(gfx->frameBuffers[3]->shaderResourceViews[0].Get(), { 256, 144 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
		ImGui::Image(gfx->frameBuffers[4]->shaderResourceViews[0].Get(), { 256, 144 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
		ImGui::Image(gfx->frameBuffers[5]->shaderResourceViews[0].Get(), { 256, 144 }, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
	}
	ImGui::End();
}
