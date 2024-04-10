#include <tchar.h>
#include "SceneGame.h"
#include "Library/Framework.h"
#include "Library/ImGui/Include/imgui.h"
#include "Library/ImGui/ConsoleData.h"
#include "Library/Input/InputManager.h"
#include "Library/Text/DispString.h"
#include "Library/Timer.h"
#include "Library/MemoryLeak.h"
#include "Library/Effekseer/EffectManager.h"

#include "PlayerManager.h"
#include "EnemyManager.h"
#include "EnemySlime.h"
#include "EnemyDragon.h"
#include "SceneManager.h"
#include "DamageTextManager.h"
#include "StageManager.h"
#include "StageMain.h"
#include "LightManager.h"
#include "Library/3D/DebugPrimitive.h"
#include "Library/3D/LineRenderer.h"

CONST LONG SHADOWMAP_WIDTH{ 1024 };
CONST LONG SHADOWMAP_HEIGHT{ 1024 };
CONST float SHADOWMAP_DRAWRECT{ 30 };

// 初期化
void SceneGame::Initialize()
{
	//directionalLight = std::make_unique<Light>();
	//directionalLight->SetColor({ 1,1,1,1 });
	//directionalLight->SetDirection({ 0,-1,-1 });

	skyMap = new SkyMap(L"Data/Texture/kloppenheim_05_puresky_4k.hdr");

	primitive2d = new_ Primitive2D();
	sprite = new_ Sprite(L"Data/Texture/Title.png");
	graphicsSpriteBatch = new_ GraphicsSpriteBatch(L"Data/Texture/Nessie.png", 2048);
	sprite3D = new_ Sprite3D(L"Data/Texture/Nessie.png");

	Player* player = new_ Player();
	PlayerManager::Instance().Register(player);

	// ステージ初期化
	// ステージ初期化
	StageManager& stageManager = StageManager::Instance();
	StageMain* stageMain = new_ StageMain();
	stageManager.Register(stageMain);


	// エネミー初期化
	for (int i = 0; i < 2; i++)
	{
		EnemySlime* slime = new_ EnemySlime();
		slime->SetPosition(DirectX::XMFLOAT3(i * 2.0f, 0, -5));
		EnemyManager::Instance().Register(slime);
	}
	EnemyDragon* dragon = new_ EnemyDragon();
	dragon->SetPosition(DirectX::XMFLOAT3(0, 0, -10));
	EnemyManager::Instance().Register(dragon);

	// --- カメラ初期設定 ---
	Camera::Instance().SetLookAt(
		DirectX::XMFLOAT3(0, 5, 20),		// カメラ座標
		DirectX::XMFLOAT3(-30, 0, -30),		// ターゲット(設定しても意味ない)
		DirectX::XMFLOAT3(0, 1, 0)			// 上方向ベクトル
	);
	Camera::Instance().SetAngle({ DirectX::XMConvertToRadians(15),DirectX::XMConvertToRadians(180), 0 });

	Camera::Instance().cameraType = Camera::CAMERA::TARGET_PLAYER;

	// ゲージ
	gauge = new Primitive2D();

	// ライト
	Light* directionLight = new Light(LightType::Directional);
	directionLight->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
	LightManager::Instance().Register(directionLight);

	// 点光源追加
	{
		Light* light = new Light(LightType::Point);
		light->SetPosition(DirectX::XMFLOAT3(2, 1, 0));
		light->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
		light->SetRange(3.0f);
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

}

// 終了化
void SceneGame::Finalize()
{
	delete primitive2d;
	delete sprite;
	delete graphicsSpriteBatch;
	delete sprite3D;

	PlayerManager::Instance().Clear();

	// エネミー終了化
	EnemyManager::Instance().Clear();

	DamageTextManager::Instance().Clear();

	StageManager::Instance().Clear();

	delete gauge;
}

// 更新
void SceneGame::Update()
{
	// カメラコントローラー更新処理
	DirectX::XMFLOAT3 target = PlayerManager::Instance().GetPlayer(0)->GetPosition();
	target.y += 1.0f;
	Camera::Instance().SetTarget(target);
	Camera::Instance().Update();

	PlayerManager::Instance().Update();

	EnemyManager::Instance().Update();

	DamageTextManager::Instance().Update();

	StageManager::Instance().Update();

	if (InputManager::Instance().GetKeyPressed(Keyboard::T))
	{
		SceneManager::Instance().ChangeScene(new SceneTitle);
	}

}

// 描画
void SceneGame::Render()
{
	// --- Graphics 取得 ---
	Graphics& gfx = Graphics::Instance();


	gfx.bloomBuffer->Clear();


	gfx.bloomBuffer->Activate();

	skyMap->Render();

	// --- ImGuiのデバッグ適用処理 ---
	gfx.SetRasterizer(static_cast<RASTERIZER_STATE>(rasterizerSelect));

	gfx.SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);


	// primitive2d
	{
		if (drawPrimitive2d)
		{
			primitive2d->Render(primitive2dPosition.x, primitive2dPosition.y, primitive2dSize.x, primitive2dSize.y, primitive2dColor.x, primitive2dColor.y, primitive2dColor.z, primitive2dColor.w, primitive2dAngle);
		}
	}

	// sprite
	{
		if (drawSprite)
		{
			sprite->Render(spritePosition.x, spritePosition.y, spriteSize.x, spriteSize.y, spriteColor.x, spriteColor.y, spriteColor.z, spriteColor.w, spriteAngle);
		}
	}

	// graphicsSpriteBatch
	{
		if (drawGraphicsSpriteBatch)
		{
			float x = 0;
			float y = 0;
			graphicsSpriteBatch->begin();
			for (size_t i = 0; i < batchCount; i++)
			{
				graphicsSpriteBatch->render(x, static_cast<float>(static_cast<int>(y) % 720), 64, 64, 1, 1, 1, 1, 0, 270 * 0, 270 * 0, 270, 270);
				x += 32;
				if (x > 1280 - 64)
				{
					x = 0;
					y += 24;
				}
			}
			graphicsSpriteBatch->end();
		}
	}



	Graphics::SceneConstants data{};
	XMMATRIX viewProjection = XMLoadFloat4x4(&Camera::Instance().GetView()) * XMLoadFloat4x4(&Camera::Instance().GetProjection());
	DirectX::XMStoreFloat4x4(&data.viewProjection, viewProjection);	// ビュー　プロジェクション　変換行列をまとめる
	
	LightManager::Instance().PushLightData(data);

	data.cameraPosition = { Camera::Instance().GetEye().x, Camera::Instance().GetEye().y, Camera::Instance().GetEye().z, 0};
	gfx.deviceContext->UpdateSubresource(gfx.constantBuffer.Get(), 0, 0, &data, 0, 0);
	gfx.deviceContext->VSSetConstantBuffers(1, 1, gfx.constantBuffer.GetAddressOf());
	gfx.deviceContext->PSSetConstantBuffers(1, 1, gfx.constantBuffer.GetAddressOf());

	StageManager::Instance().Render();

	PlayerManager::Instance().Render();

	EnemyManager::Instance().Render();


	

	// sprite3D
	{
		// スケール行列を作成
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(s3scale.x, s3scale.y, s3scale.z);
		// 回転行列を作成
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(s3angle.x), DirectX::XMConvertToRadians(s3angle.y), DirectX::XMConvertToRadians(s3angle.z));
		// 位置行列を作成
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(s3position.x, s3position.y, s3position.z);

		// ４つの行列を組み合わせ、ワールド行列を作成
		DirectX::XMMATRIX W = S * R * T;
		// 計算したワールド行列を取り出す
		DirectX::XMStoreFloat4x4(&s3transform, W);

		sprite3D->Render(s3transform, {1,1,1,1}, 0,0, 270, 270);
	}

	EffectManager::Instance().Render(Camera::Instance().GetView(), Camera::Instance().GetProjection());

	// エネミーHPゲージ描画
	RenderEnemyGauge();

	DamageTextManager::Instance().Render();

	// camear デバッグ
	if (Camera::Instance().drawFocusSphere)
	{
		DirectX::XMMATRIX S{ DirectX::XMMatrixScaling(0.1, 0.1, 0.1) };
		DirectX::XMMATRIX R{ DirectX::XMMatrixRotationRollPitchYaw(0,0,0) };
		DirectX::XMMATRIX T{ DirectX::XMMatrixTranslation(Camera::Instance().GetFocus().x, Camera::Instance().GetFocus().y, Camera::Instance().GetFocus().z) };

		DirectX::XMFLOAT4X4 world;
		DirectX::XMStoreFloat4x4(&world, S * R * T);
		Camera::Instance().focusSphere->Render(world, { 1,0,0,1 });
	}


	PlayerManager::Instance().DrawDebugGui();
	EnemyManager::Instance().DrawDebugGui();

	//directionalLight->DrawDebugGUI();
	LightManager::Instance().DrawDebugPrimitive();
	LightManager::Instance().DrawDebugGui();

	// --- テキスト描画 ---
	DispString::Instance().Draw(L"HOSHIN LIB", { 800, 60 }, 48, TEXT_ALIGN::MIDDLE, { 0, 0, 0, 1 });

	// --- デバッグ描画 ---
	DebugPrimitive::Instance().Render();
	LineRenderer::Instance().Render();

	

	gfx.bloomBuffer->DeActivate();

	gfx.SetDepthStencil(DEPTHSTENCIL_STATE::ZT_OFF_ZW_OFF);

#if 1
	// --- 高輝度抽出 ---
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
	gfx.deviceContext->UpdateSubresource(gfx.constantBuffers[2].Get(), 0, 0, &gfx.colorFilterConstant, 0, 0);
	gfx.deviceContext->PSSetConstantBuffers(3, 1, gfx.constantBuffers[2].GetAddressOf());
	
	gfx.frameBuffers[5]->Activate();
	gfx.bitBlockTransfer->blit(gfx.frameBuffers[4]->shaderResourceViews[0].GetAddressOf(), 0, 1, gfx.pixelShaders[static_cast<size_t>(PS_TYPE::ColorFilter_PS)].Get());
	gfx.frameBuffers[5]->DeActivate();

	
	gfx.bitBlockTransfer->blit(gfx.frameBuffers[5]->shaderResourceViews[0].GetAddressOf(), 0, 1);


#else
	gfx.bitBlockTransfer->blit(gfx.frameBuffers[0]->shaderResourceViews[0].GetAddressOf(), 0, 1);
#endif

	// --- デバッグ描画 ---
	DrawDebugGUI();
}

// デバッグ描画
void SceneGame::DrawDebugGUI()
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
				ImGui::EndMenu();
			}
			
			if (ImGui::MenuItem("Open", "Ctrl+O")) {
				OPENFILENAME ofn;       // ファイル選択用の構造体
				TCHAR szFile[260] = { 0 };  // ファイルパスを格納するバッファ

				// 構造体の初期化
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lpstrFilter = _TEXT("animファイル(*.anim)\0*.anim\0") _TEXT("全てのファイル(*.*)\0*.*\0");
				ofn.lStructSize = sizeof(ofn);
				ofn.lpstrFile = szFile;
				ofn.lpstrFile[0] = '\0';
				ofn.nMaxFile = sizeof(szFile);
				ofn.nFilterIndex = 1;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

				GetOpenFileName(&ofn);
			}
			if (ImGui::MenuItem("Save", "Ctrl+S")) {
			}
			if (ImGui::MenuItem("Save as..")) {
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}


	Graphics* gfx = &Graphics::Instance();
	ConsoleData* console = &ConsoleData::Instance();

	// -- camera ---
	Camera::Instance().DrawDebugGui();


	// --- Objects ---
	if (ImGui::Begin("Objects", nullptr, ImGuiWindowFlags_None))
	{
		// --- Primitive2D ---
		{
			if (ImGui::CollapsingHeader("Primitive2D", ImGuiTreeNodeFlags_None))
			{
				// 描画
				ImGui::Checkbox("Draw", &drawPrimitive2d);
				// 位置
				ImGui::DragFloat2("Position", &primitive2dPosition.x, 1.0f);
				// サイズ
				ImGui::DragFloat2("Size", &primitive2dSize.x, 1.0f);
				// 回転
				ImGui::DragFloat("Angle", &primitive2dAngle);
				// カラー
				ImGui::ColorPicker4("Color", &primitive2dColor.x);
			}
		}
		// --- Sprite ---
		{
			if (ImGui::CollapsingHeader("Sprite", ImGuiTreeNodeFlags_None))
			{
				// 描画
				ImGui::Checkbox("Draw", &drawSprite);
				// 位置
				ImGui::DragFloat3("Position", &s3position.x, 1.0f);
				// サイズ
				ImGui::DragFloat3("Size", &s3scale.x, 1.0f);
				// 回転
				ImGui::DragFloat3("Angle", &s3angle.x, 1.0f);
				// カラー
				ImGui::ColorPicker4("Color", &spriteColor.x);
			}
		}
		// --- GraphicsSpriteBatch ---
		{
			if (ImGui::CollapsingHeader("GraphicsSpriteBatch", ImGuiTreeNodeFlags_None))
			{
				// 描画
				ImGui::Checkbox("Draw", &drawGraphicsSpriteBatch);
				// 描画数
				ImGui::InputInt("Count", &batchCount);
			}
		}
	}
	ImGui::End();

	ImGui::Begin("ColorFilter");
	{
		ImGui::SliderFloat("threshould", &gfx->luminanceExtractionConstant.threshould, 0.0f, 1.0f);
		ImGui::SliderFloat("intensity", &gfx->luminanceExtractionConstant.intensity, 0.0f, 10.0f);
		ImGui::SliderFloat("HueShift", &gfx->colorFilterConstant.hueShift, 0.0f, 360.0f);
		ImGui::SliderFloat("saturation", &gfx->colorFilterConstant.saturation, 0.0f, 2.0f);
		ImGui::SliderFloat("brightness", &gfx->colorFilterConstant.brightness, 0.0f, 2.0f);

		ImGui::DragFloat("gaussianPower", &gaussianPower, 0.1f, 0.1f, 16.0f);

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

// エネミーHPゲージ描画
void SceneGame::RenderEnemyGauge()
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

	// 全ての敵の頭上にHPゲージを表示
	EnemyManager& enemyManager = EnemyManager::Instance();
	int enemyCount = enemyManager.GetEnemyCount();

	for (int i = 0; i < enemyCount; i++)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);

		// エネミー頭上の座標取得
		DirectX::XMFLOAT3 worldPosition = enemy->GetPosition();
		worldPosition.y += enemy->GetHeight();
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
		DirectX::XMStoreFloat2(&screenPosition, ScreenPosition);

		// ゲージの長さ
		const float gaugeWidth = 60.0f;
		const float gaugeHeight = 15.0f;

		float healthRate = enemy->GetHealth() / static_cast<float>(enemy->GetMaxHealth());

		// ゲージ描画
		gauge->Render(
			screenPosition.x - gaugeWidth * 0.5f, screenPosition.y - gaugeHeight * 0.5f,
			gaugeWidth * healthRate, gaugeHeight,
			1, 0, 0, 1,
			0
		);
	}

	// エネミー配置処理
	InputManager& input = InputManager::Instance();
	if (input.GetMousePressed(MOUSEBUTTON_STATE::middleButton))
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
			// 敵を配置
			EnemySlime* slime = new EnemySlime();
			slime->SetPosition(hit.position);
			EnemyManager::Instance().Register(slime);
		}
	}
}
