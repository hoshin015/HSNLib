#include <tchar.h>
#include "SceneSpawnEditor.h"
#include "SceneManager.h"
#include "Library/Input/InputManager.h"
#include "Library/ImGui/Include/imgui.h"
#include "Library/Timer.h"
#include "StageManager.h"
#include "StageContext.h"
#include "LightManager.h"
#include "SpinningTopEnemyManager.h"
#include "ObstacleManager.h"
#include "StEnemy.h"
#include "Library/3D/DebugPrimitive.h"
#include "Library/3D/LineRenderer.h"
#include "Library/ImGui/ConsoleData.h"

#include "DataManager.h"
#include <random>

void SceneSpawnEditor::Initialize()
{
	// json のデータを spawnEreaData に読みこむ
	DataManager::Instance().LoadSpawnEreaData();

	// 障害物生成
	for (int i = 0; i < 3; i++)
	{
		//Obstacle* obstacle = new_ Obstacle("Data/FBX/cylinder/cylinder.fbx");
		//obstacle->SetPosition({ i * 10.0f - 10.0f, 0, -6.0f });
		//ObstacleManager::Instance().Register(obstacle);
	}

	// ステージ初期化
	StageManager& stageManager = StageManager::Instance();
	StageContext* stageMain = new_ StageContext();
	stageManager.Register(stageMain);

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
	Camera::Instance().SetAngle({ DirectX::XMConvertToRadians(60),DirectX::XMConvertToRadians(180), 0 });

	Camera::Instance().cameraType = Camera::CAMERA::LOCK;
}

void SceneSpawnEditor::Finalize()
{
	StageManager::Instance().Clear();

	LightManager::Instance().Clear();

	SpinningTopEnemyManager::Instance().Clear();

	ObstacleManager::Instance().Clear();
}

void SceneSpawnEditor::Update()
{
	// カメラコントローラー更新処理
	Camera::Instance().Update();

	StageManager::Instance().Update();

	SpinningTopEnemyManager::Instance().Update();

	ObstacleManager::Instance().Update();

	UpdateGeneratePosition();
}

void SceneSpawnEditor::Render()
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
void SceneSpawnEditor::DrawDebugGUI()
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
				if (ImGui::MenuItem("EnemyEditor"))
				{
					SceneManager::Instance().ChangeScene(new SceneLoading(new SceneSpawnEditor));
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	// debugPrimitive
	DebugPrimitive::Instance().AddSphere(targetPosition, 0.5f, { 0,1,0,1 });	// ターゲット座標

	// スポーンエリアのレベルデザイン
	static int selectEnemyKind = 0;

	ImGui::Begin(u8"スポーンエリアレベルデザイン");

	ImGui::InputInt(u8"敵生成数", &enemyNum);
	
	if (ImGui::Button(u8"クリア"))
	{
		SpinningTopEnemyManager::Instance().Clear();
	}

	ImGui::Separator();

	if (ImGui::Button(u8"保存"))
	{
		DataManager::Instance().SaveSpawnEreaData();
		ConsoleData::Instance().logs.push_back(u8"スポーンエリアデータ保存完了");
	}

	for (int i = 0; i < EREA_NUM; i++)
	{
		if (ImGui::CollapsingHeader(("EREA - " + std::to_string(i)).c_str(), ImGuiTreeNodeFlags_None))
		{
			ImGui::PushID((u8"EREA" + std::to_string(i)).c_str());

			// 位置
			ImGui::InputFloat3(u8"ポジション", &enemySpawnErea[i].position.x);
			ImGui::DragFloat(u8"半径", &enemySpawnErea[i].radius, 0.1f);
			
			if(ImGui::Button(u8"ポジション更新"))
			{
				enemySpawnErea[i].position = targetPosition;
			}

			ImGui::SameLine();

			if (ImGui::Button(u8"敵生成"))
			{
				// 乱数生成器の設定
				std::random_device rd;  // 真の乱数生成器を初期化
				std::mt19937 gen(rd()); // メルセンヌ・ツイスタ乱数生成器を初期化
				std::uniform_real_distribution<float> dis(0.0f, enemySpawnErea[i].radius); // 0からenemySpawnErea[i].radiusの間の一様乱数生成器を作成

				// ランダムなfloat型の値を取得
				float randomValue = dis(gen);

				for (int j = 0; j < enemyNum; j++)
				{
					StEnemy* enemy = new_ StEnemy(ENEMY_0);

					float xPos = enemySpawnErea[i].position.x;
					float zPos = enemySpawnErea[i].position.z;

					float rad = DirectX::XMConvertToRadians(rand() % 360);
					float dist = dis(gen);

					xPos += cosf(rad) * dist;
					zPos += sinf(rad) * dist;

					enemy->SetPosition({ xPos, 10, zPos });

					// スポーン座標設定
					enemy->spawnPosition = { enemy->GetPosition().x, 0, enemy->GetPosition().z };
					SpinningTopEnemyManager::Instance().Register(enemy);
				}
			}
			ImGui::PopID();

			DebugPrimitive::Instance().AddCylinder(enemySpawnErea[i].position, enemySpawnErea[i].radius, 0.4f, { 1,0,0,1 });
		}
	}
	

	ImGui::End();
}

// 生成位置更新
void SceneSpawnEditor::UpdateGeneratePosition()
{
	// --- Graphics 取得 ---
	Graphics& gfx = Graphics::Instance();

	// ビューポート
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	gfx.deviceContext->RSGetViewports(&numViewports, &viewport);

	// 変換行列
	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&Camera::Instance().GetView());
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&Camera::Instance().GetProjection());
	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();
	// エネミー配置処理
	InputManager& input = InputManager::Instance();
	if (input.GetMousePress(MOUSEBUTTON_STATE::rightButton))
	{
		// マウスカーソル座標を取得
		DirectX::XMFLOAT3 screenPosition;
		screenPosition.x = static_cast<int>(input.GetCursorPosX());
		screenPosition.y = static_cast<int>(input.GetCursorPosY());

		DirectX::XMVECTOR ScreenPosition, WorldPosition;

		// レイの始点を算出
		screenPosition.z = 0.0f;
		ScreenPosition = DirectX::XMLoadFloat3(&screenPosition);

		WorldPosition = DirectX::XMVector3Unproject(
			ScreenPosition,
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
		DirectX::XMFLOAT3 rayStart;
		DirectX::XMStoreFloat3(&rayStart, WorldPosition);

		// レイの終点を算出
		screenPosition.z = 1.0f;
		ScreenPosition = DirectX::XMLoadFloat3(&screenPosition);

		WorldPosition = DirectX::XMVector3Unproject(
			ScreenPosition,
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
		DirectX::XMFLOAT3 rayEnd;
		DirectX::XMStoreFloat3(&rayEnd, WorldPosition);

		// レイキャスト
		HitResult hit;
		if (StageManager::Instance().RayCast(rayStart, rayEnd, hit))
		{
			targetPosition = hit.position;
		}
	}
}
