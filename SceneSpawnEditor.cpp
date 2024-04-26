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
	// json �̃f�[�^�� spawnEreaData �ɓǂ݂���
	DataManager::Instance().LoadSpawnEreaData();

	// ��Q������
	for (int i = 0; i < 3; i++)
	{
		//Obstacle* obstacle = new_ Obstacle("Data/FBX/cylinder/cylinder.fbx");
		//obstacle->SetPosition({ i * 10.0f - 10.0f, 0, -6.0f });
		//ObstacleManager::Instance().Register(obstacle);
	}

	// �X�e�[�W������
	StageManager& stageManager = StageManager::Instance();
	StageContext* stageMain = new_ StageContext();
	stageManager.Register(stageMain);

	// ���C�g������
	Light* directionLight = new Light(LightType::Directional);
	directionLight->SetDirection(DirectX::XMFLOAT3(0.5, -1, -1));
	directionLight->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
	LightManager::Instance().Register(directionLight);

	// �X�J�C�}�b�v
	skyMap = std::make_unique<SkyMap>(L"Data/Texture/kloppenheim_05_puresky_4k.hdr");


	// �J���������ݒ�
	Camera::Instance().SetLookAt(
		DirectX::XMFLOAT3(0, 90, 60),		// �J�������W
		DirectX::XMFLOAT3(-90, 0, -30),		// �^�[�Q�b�g(�ݒ肵�Ă��Ӗ��Ȃ�)
		DirectX::XMFLOAT3(0, 1, 0)			// ������x�N�g��
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
	// �J�����R���g���[���[�X�V����
	Camera::Instance().Update();

	StageManager::Instance().Update();

	SpinningTopEnemyManager::Instance().Update();

	ObstacleManager::Instance().Update();

	UpdateGeneratePosition();
}

void SceneSpawnEditor::Render()
{
	// --- Graphics �擾 ---
	Graphics& gfx = Graphics::Instance();

	// shadowMap
	{
		gfx.SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);

		gfx.shadowBuffer->Clear();
		gfx.shadowBuffer->Activate();


		// �J�����p�����[�^�ݒ�
		{
			// ���s��������J�����ʒu���쐬���A�������猴�_�̈ʒu������悤�Ɏ����s��𐶐�
			DirectX::XMFLOAT3 dir = LightManager::Instance().GetLight(0)->GetDirection();
			DirectX::XMVECTOR LightPosition = DirectX::XMLoadFloat3(&dir);
			LightPosition = DirectX::XMVectorScale(LightPosition, -250.0f);
			DirectX::XMMATRIX V = DirectX::XMMatrixLookAtLH(LightPosition,
				DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
				DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

			// �V���h�E�}�b�v�ɕ`�悵�����͈͂̎ˉe�s��𐶐�
			DirectX::XMMATRIX P = DirectX::XMMatrixOrthographicLH(gfx.shadowDrawRect, gfx.shadowDrawRect, 0.1f, 1000.0f);
			XMMATRIX viewProjection = V * P;
			DirectX::XMStoreFloat4x4(&gfx.shadowMapData.lightViewProjection, viewProjection);	// �r���[�@�v���W�F�N�V�����@�ϊ��s����܂Ƃ߂�
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
	DirectX::XMStoreFloat4x4(&data.viewProjection, viewProjection);	// �r���[�@�v���W�F�N�V�����@�ϊ��s����܂Ƃ߂�

	LightManager::Instance().PushLightData(data);

	data.cameraPosition = { Camera::Instance().GetEye().x, Camera::Instance().GetEye().y, Camera::Instance().GetEye().z, 0 };
	gfx.deviceContext->UpdateSubresource(gfx.constantBuffer.Get(), 0, 0, &data, 0, 0);
	gfx.deviceContext->VSSetConstantBuffers(1, 1, gfx.constantBuffer.GetAddressOf());
	gfx.deviceContext->PSSetConstantBuffers(1, 1, gfx.constantBuffer.GetAddressOf());

	// shadowMap �̃o�C���h
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

	// --- �f�o�b�O�`�� ---
	DebugPrimitive::Instance().Render();
	LineRenderer::Instance().Render();

	// --- �f�o�b�O�`�� ---
	DrawDebugGUI();

}

// �f�o�b�O�`��
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
	DebugPrimitive::Instance().AddSphere(targetPosition, 0.5f, { 0,1,0,1 });	// �^�[�Q�b�g���W

	// �X�|�[���G���A�̃��x���f�U�C��
	static int selectEnemyKind = 0;

	ImGui::Begin(u8"�X�|�[���G���A���x���f�U�C��");

	ImGui::InputInt(u8"�G������", &enemyNum);
	
	if (ImGui::Button(u8"�N���A"))
	{
		SpinningTopEnemyManager::Instance().Clear();
	}

	ImGui::Separator();

	if (ImGui::Button(u8"�ۑ�"))
	{
		DataManager::Instance().SaveSpawnEreaData();
		ConsoleData::Instance().logs.push_back(u8"�X�|�[���G���A�f�[�^�ۑ�����");
	}

	for (int i = 0; i < EREA_NUM; i++)
	{
		if (ImGui::CollapsingHeader(("EREA - " + std::to_string(i)).c_str(), ImGuiTreeNodeFlags_None))
		{
			ImGui::PushID((u8"EREA" + std::to_string(i)).c_str());

			// �ʒu
			ImGui::InputFloat3(u8"�|�W�V����", &enemySpawnErea[i].position.x);
			ImGui::DragFloat(u8"���a", &enemySpawnErea[i].radius, 0.1f);
			
			if(ImGui::Button(u8"�|�W�V�����X�V"))
			{
				enemySpawnErea[i].position = targetPosition;
			}

			ImGui::SameLine();

			if (ImGui::Button(u8"�G����"))
			{
				// ����������̐ݒ�
				std::random_device rd;  // �^�̗����������������
				std::mt19937 gen(rd()); // �����Z���k�E�c�C�X�^�����������������
				std::uniform_real_distribution<float> dis(0.0f, enemySpawnErea[i].radius); // 0����enemySpawnErea[i].radius�̊Ԃ̈�l������������쐬

				// �����_����float�^�̒l���擾
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

					// �X�|�[�����W�ݒ�
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

// �����ʒu�X�V
void SceneSpawnEditor::UpdateGeneratePosition()
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
	// �G�l�~�[�z�u����
	InputManager& input = InputManager::Instance();
	if (input.GetMousePress(MOUSEBUTTON_STATE::rightButton))
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
			targetPosition = hit.position;
		}
	}
}
