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
#include "StEnemy01.h"
#include "Library/3D/DebugPrimitive.h"
#include "Library/3D/LineRenderer.h"

void SceneSpinningTop::Initialize()
{
	for (int i = 0; i < 6; i++)
	{
		StEnemy01* slime = new_ StEnemy01();
		slime->SetPosition({ i * 2.0f - 6, 0, 0 });
		SpinningTopEnemyManager::Instance().Register(slime);
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

	// �J���������ݒ�
	Camera::Instance().SetLookAt(
		DirectX::XMFLOAT3(0, 50, 30),		// �J�������W
		DirectX::XMFLOAT3(-90, 0, -30),		// �^�[�Q�b�g(�ݒ肵�Ă��Ӗ��Ȃ�)
		DirectX::XMFLOAT3(0, 1, 0)			// ������x�N�g��
	);
	Camera::Instance().SetAngle({ DirectX::XMConvertToRadians(60), DirectX::XMConvertToRadians(180), 0 });

	Camera::Instance().cameraType = Camera::CAMERA::FREE;
}

void SceneSpinningTop::Finalize()
{
	StageManager::Instance().Clear();

	LightManager::Instance().Clear();

	SpinningTopEnemyManager::Instance().Clear();
}

void SceneSpinningTop::Update()
{
	// �J�����R���g���[���[�X�V����
	Camera::Instance().Update();


	InputManager& input = InputManager::Instance();

	if (input.GetKeyPressed(Keyboard::Enter))
	{
		SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
	}

	StageManager::Instance().Update();

	SpinningTopEnemyManager::Instance().Update();
}

void SceneSpinningTop::Render()
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

		SpinningTopEnemyManager::Instance().Render();

		gfx.shadowBuffer->DeActivate();
	}

	// --- ImGui�̃f�o�b�O�K�p���� ---
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

	// --- �f�o�b�O�`�� ---
	DebugPrimitive::Instance().Render();
	LineRenderer::Instance().Render();

	// --- �f�o�b�O�`�� ---
	DrawDebugGUI();

}

// �f�o�b�O�`��
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
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	SpinningTopEnemyManager::Instance().DrawDebugGui();
}
