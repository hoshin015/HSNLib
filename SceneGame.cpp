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

// ������
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

	// �X�e�[�W������
	// �X�e�[�W������
	StageManager& stageManager = StageManager::Instance();
	StageMain* stageMain = new_ StageMain();
	stageManager.Register(stageMain);


	// �G�l�~�[������
	for (int i = 0; i < 2; i++)
	{
		EnemySlime* slime = new_ EnemySlime();
		slime->SetPosition(DirectX::XMFLOAT3(i * 2.0f, 0, -5));
		EnemyManager::Instance().Register(slime);
	}
	EnemyDragon* dragon = new_ EnemyDragon();
	dragon->SetPosition(DirectX::XMFLOAT3(0, 0, -10));
	EnemyManager::Instance().Register(dragon);

	// --- �J���������ݒ� ---
	Camera::Instance().SetLookAt(
		DirectX::XMFLOAT3(0, 5, 20),		// �J�������W
		DirectX::XMFLOAT3(-30, 0, -30),		// �^�[�Q�b�g(�ݒ肵�Ă��Ӗ��Ȃ�)
		DirectX::XMFLOAT3(0, 1, 0)			// ������x�N�g��
	);
	Camera::Instance().SetAngle({ DirectX::XMConvertToRadians(15),DirectX::XMConvertToRadians(180), 0 });

	Camera::Instance().cameraType = Camera::CAMERA::TARGET_PLAYER;

	// �Q�[�W
	gauge = new Primitive2D();

	// ���C�g
	Light* directionLight = new Light(LightType::Directional);
	directionLight->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
	LightManager::Instance().Register(directionLight);

	// �_�����ǉ�
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
	// �X�|�b�g���C�g��ǉ�
	{
		Light* light = new Light(LightType::Spot);
		light->SetPosition(DirectX::XMFLOAT3(-3, 2, 0));
		light->SetColor(DirectX::XMFLOAT4(1, 0, 0, 1));
		light->SetDirection(DirectX::XMFLOAT3(+1, -1, 0));
		light->SetRange(4.0f);
		LightManager::Instance().Register(light);
	}

}

// �I����
void SceneGame::Finalize()
{
	delete primitive2d;
	delete sprite;
	delete graphicsSpriteBatch;
	delete sprite3D;

	PlayerManager::Instance().Clear();

	// �G�l�~�[�I����
	EnemyManager::Instance().Clear();

	DamageTextManager::Instance().Clear();

	StageManager::Instance().Clear();

	delete gauge;
}

// �X�V
void SceneGame::Update()
{
	// �J�����R���g���[���[�X�V����
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

// �`��
void SceneGame::Render()
{
	// --- Graphics �擾 ---
	Graphics& gfx = Graphics::Instance();


	gfx.bloomBuffer->Clear();


	gfx.bloomBuffer->Activate();

	skyMap->Render();

	// --- ImGui�̃f�o�b�O�K�p���� ---
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
	DirectX::XMStoreFloat4x4(&data.viewProjection, viewProjection);	// �r���[�@�v���W�F�N�V�����@�ϊ��s����܂Ƃ߂�
	
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
		// �X�P�[���s����쐬
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(s3scale.x, s3scale.y, s3scale.z);
		// ��]�s����쐬
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(s3angle.x), DirectX::XMConvertToRadians(s3angle.y), DirectX::XMConvertToRadians(s3angle.z));
		// �ʒu�s����쐬
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(s3position.x, s3position.y, s3position.z);

		// �S�̍s���g�ݍ��킹�A���[���h�s����쐬
		DirectX::XMMATRIX W = S * R * T;
		// �v�Z�������[���h�s������o��
		DirectX::XMStoreFloat4x4(&s3transform, W);

		sprite3D->Render(s3transform, {1,1,1,1}, 0,0, 270, 270);
	}

	EffectManager::Instance().Render(Camera::Instance().GetView(), Camera::Instance().GetProjection());

	// �G�l�~�[HP�Q�[�W�`��
	RenderEnemyGauge();

	DamageTextManager::Instance().Render();

	// camear �f�o�b�O
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

	// --- �e�L�X�g�`�� ---
	DispString::Instance().Draw(L"HOSHIN LIB", { 800, 60 }, 48, TEXT_ALIGN::MIDDLE, { 0, 0, 0, 1 });

	// --- �f�o�b�O�`�� ---
	DebugPrimitive::Instance().Render();
	LineRenderer::Instance().Render();

	

	gfx.bloomBuffer->DeActivate();

	gfx.SetDepthStencil(DEPTHSTENCIL_STATE::ZT_OFF_ZW_OFF);

#if 1
	// --- ���P�x���o ---
	gfx.deviceContext->UpdateSubresource(gfx.constantBuffers[4].Get(), 0, 0, &gfx.luminanceExtractionConstant, 0, 0);
	gfx.deviceContext->PSSetConstantBuffers(0, 1, gfx.constantBuffers[4].GetAddressOf());
	gfx.frameBuffers[1]->Activate();
	gfx.bitBlockTransfer->blit(gfx.bloomBuffer->shaderResourceViews[0].GetAddressOf(), 0, 2, gfx.pixelShaders[static_cast<size_t>(PS_TYPE::LuminanceExtraction_PS)].Get());
	gfx.frameBuffers[1]->DeActivate();
	gfx.bitBlockTransfer->blit(gfx.frameBuffers[0]->shaderResourceViews[0].GetAddressOf(), 0, 1);

	// --- �K�E�V�A���t�B���^ ---
	gfx.CalcWeightsTableFromGaussian(gaussianPower);
	gfx.deviceContext->UpdateSubresource(gfx.constantBuffers[3].Get(), 0, 0, &gfx.gaussianConstant, 0, 0);
	gfx.deviceContext->PSSetConstantBuffers(0, 1, gfx.constantBuffers[3].GetAddressOf());

	gfx.frameBuffers[2]->Activate();
	gfx.bitBlockTransfer->blit(gfx.frameBuffers[1]->shaderResourceViews[0].GetAddressOf(), 0, 1, gfx.pixelShaders[static_cast<size_t>(PS_TYPE::GaussianBlur_PS)].Get(), gfx.vertexShaders[static_cast<size_t>(VS_TYPE::GaussianBlurX_VS)].Get());
	gfx.frameBuffers[2]->DeActivate();

	gfx.frameBuffers[3]->Activate();
	gfx.bitBlockTransfer->blit(gfx.frameBuffers[2]->shaderResourceViews[0].GetAddressOf(), 0, 1, gfx.pixelShaders[static_cast<size_t>(PS_TYPE::GaussianBlur_PS)].Get(), gfx.vertexShaders[static_cast<size_t>(VS_TYPE::GaussianBlurY_VS)].Get());
	gfx.frameBuffers[3]->DeActivate();

	// --- �u���[�����Z ---
	ID3D11ShaderResourceView* shvs[2] =
	{
		gfx.bloomBuffer->shaderResourceViews[0].Get(),
		gfx.frameBuffers[3]->shaderResourceViews[0].Get()
	};
	gfx.frameBuffers[4]->Activate();
	gfx.bitBlockTransfer->blit(shvs, 0, 2, gfx.pixelShaders[static_cast<size_t>(PS_TYPE::BloomFinalPass_PS)].Get());
	gfx.frameBuffers[4]->DeActivate();

	// --- �J���[�t�B���^�[ ---
	gfx.deviceContext->UpdateSubresource(gfx.constantBuffers[2].Get(), 0, 0, &gfx.colorFilterConstant, 0, 0);
	gfx.deviceContext->PSSetConstantBuffers(3, 1, gfx.constantBuffers[2].GetAddressOf());
	
	gfx.frameBuffers[5]->Activate();
	gfx.bitBlockTransfer->blit(gfx.frameBuffers[4]->shaderResourceViews[0].GetAddressOf(), 0, 1, gfx.pixelShaders[static_cast<size_t>(PS_TYPE::ColorFilter_PS)].Get());
	gfx.frameBuffers[5]->DeActivate();

	
	gfx.bitBlockTransfer->blit(gfx.frameBuffers[5]->shaderResourceViews[0].GetAddressOf(), 0, 1);


#else
	gfx.bitBlockTransfer->blit(gfx.frameBuffers[0]->shaderResourceViews[0].GetAddressOf(), 0, 1);
#endif

	// --- �f�o�b�O�`�� ---
	DrawDebugGUI();
}

// �f�o�b�O�`��
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
				OPENFILENAME ofn;       // �t�@�C���I��p�̍\����
				TCHAR szFile[260] = { 0 };  // �t�@�C���p�X���i�[����o�b�t�@

				// �\���̂̏�����
				ZeroMemory(&ofn, sizeof(ofn));
				ofn.lpstrFilter = _TEXT("anim�t�@�C��(*.anim)\0*.anim\0") _TEXT("�S�Ẵt�@�C��(*.*)\0*.*\0");
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
				// �`��
				ImGui::Checkbox("Draw", &drawPrimitive2d);
				// �ʒu
				ImGui::DragFloat2("Position", &primitive2dPosition.x, 1.0f);
				// �T�C�Y
				ImGui::DragFloat2("Size", &primitive2dSize.x, 1.0f);
				// ��]
				ImGui::DragFloat("Angle", &primitive2dAngle);
				// �J���[
				ImGui::ColorPicker4("Color", &primitive2dColor.x);
			}
		}
		// --- Sprite ---
		{
			if (ImGui::CollapsingHeader("Sprite", ImGuiTreeNodeFlags_None))
			{
				// �`��
				ImGui::Checkbox("Draw", &drawSprite);
				// �ʒu
				ImGui::DragFloat3("Position", &s3position.x, 1.0f);
				// �T�C�Y
				ImGui::DragFloat3("Size", &s3scale.x, 1.0f);
				// ��]
				ImGui::DragFloat3("Angle", &s3angle.x, 1.0f);
				// �J���[
				ImGui::ColorPicker4("Color", &spriteColor.x);
			}
		}
		// --- GraphicsSpriteBatch ---
		{
			if (ImGui::CollapsingHeader("GraphicsSpriteBatch", ImGuiTreeNodeFlags_None))
			{
				// �`��
				ImGui::Checkbox("Draw", &drawGraphicsSpriteBatch);
				// �`�搔
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

// �G�l�~�[HP�Q�[�W�`��
void SceneGame::RenderEnemyGauge()
{
	// --- Graphics �擾 ---
	Graphics& gfx = Graphics::Instance();

	// �r���[�|�[�g
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	gfx.deviceContext->RSGetViewports(&numViewports, &viewport);

	// �ϊ��s��
	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&Camera::Instance().GetView());
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&Camera::Instance().GetProjection());
	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();

	// �S�Ă̓G�̓����HP�Q�[�W��\��
	EnemyManager& enemyManager = EnemyManager::Instance();
	int enemyCount = enemyManager.GetEnemyCount();

	for (int i = 0; i < enemyCount; i++)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);

		// �G�l�~�[����̍��W�擾
		DirectX::XMFLOAT3 worldPosition = enemy->GetPosition();
		worldPosition.y += enemy->GetHeight();
		DirectX::XMVECTOR WorldPosition = DirectX::XMLoadFloat3(&worldPosition);

		// ���[���h���W����X�N���[�����W�ւ̕ϊ�
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

		// �X�N���[�����W
		DirectX::XMFLOAT2 screenPosition;
		DirectX::XMStoreFloat2(&screenPosition, ScreenPosition);

		// �Q�[�W�̒���
		const float gaugeWidth = 60.0f;
		const float gaugeHeight = 15.0f;

		float healthRate = enemy->GetHealth() / static_cast<float>(enemy->GetMaxHealth());

		// �Q�[�W�`��
		gauge->Render(
			screenPosition.x - gaugeWidth * 0.5f, screenPosition.y - gaugeHeight * 0.5f,
			gaugeWidth * healthRate, gaugeHeight,
			1, 0, 0, 1,
			0
		);
	}

	// �G�l�~�[�z�u����
	InputManager& input = InputManager::Instance();
	if (input.GetMousePressed(MOUSEBUTTON_STATE::middleButton))
	{
		// �}�E�X�J�[�\�����W���擾
		DirectX::XMFLOAT3 screenPosition;
		screenPosition.x = static_cast<int>(input.GetCursorPosX());
		screenPosition.y = static_cast<int>(input.GetCursorPosY());

		DirectX::XMVECTOR ScreenPosition, WorldPosition;

		// ���C�̎n�_���Z�o
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

		// ���C�̏I�_���Z�o
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

		// ���C�L���X�g
		HitResult hit;
		if (StageManager::Instance().RayCast(rayStart, rayEnd, hit))
		{
			// �G��z�u
			EnemySlime* slime = new EnemySlime();
			slime->SetPosition(hit.position);
			EnemyManager::Instance().Register(slime);
		}
	}
}
