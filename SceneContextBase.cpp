#include <tchar.h>
#include "SceneContextBase.h"
#include "Library/Framework.h"
#include "Library/ImGui/Include/imgui.h"
#include "Library/ImGui/ConsoleData.h"
#include "Library/Input/InputManager.h"
#include "Library/Text/DispString.h"
#include "Library/Timer.h"
#include "Library/MemoryLeak.h"
#include "Library/Effekseer/EffectManager.h"
#include "Library/3D/LineRenderer.h"

#include "PlayerManager.h"
#include "EnemyManager.h"
#include "EnemyContextBaseSlime.h"
#include "EnemyDragon.h"
#include "SceneManager.h"
#include "DamageTextManager.h"
#include "StageManager.h"
#include "StageContext.h"
#include "StageBox.h"
#include "Library/3D/DebugPrimitive.h"
#include "LightManager.h"


// ������
void SceneContextBase::Initialize()
{
	Player* player = new_ Player();
	player->SetPosition({ 0,0,5 });
	PlayerManager::Instance().Register(player);

	// �X�e�[�W������
	StageManager& stageManager = StageManager::Instance();
	StageContext* stageMain = new_ StageContext();
	stageManager.Register(stageMain);

	// �G�l�~�[������
	for (int i = 0; i < 1; i++)
	{
		EnemyContextBaseSlime* slime = new_ EnemyContextBaseSlime();
		slime->SetPosition({i * 2.0f, 0, 0});
		EnemyManager::Instance().Register(slime);
	}
	

	// --- �J���������ݒ� ---
	Camera::Instance().SetLookAt(
		DirectX::XMFLOAT3(0, 5, -20),		// �J�������W
		DirectX::XMFLOAT3(-30, 0, -30),		// �^�[�Q�b�g(�ݒ肵�Ă��Ӗ��Ȃ�)
		DirectX::XMFLOAT3(0, 1, 0)			// ������x�N�g��
	);
	Camera::Instance().SetAngle({ DirectX::XMConvertToRadians(30), DirectX::XMConvertToRadians(180), 0 });

	Camera::Instance().cameraType = Camera::CAMERA::TARGET_PLAYER;

	// �Q�[�W
	gauge = new Primitive2D();

	// ���C�g
	Light* directionLight = new Light(LightType::Directional);
	directionLight->SetDirection(DirectX::XMFLOAT3(0.5, -1, -1));
	directionLight->SetColor(DirectX::XMFLOAT4(1, 1, 1, 1));
	LightManager::Instance().Register(directionLight);
}

// �I����
void SceneContextBase::Finalize()
{
	PlayerManager::Instance().Clear();

	// �G�l�~�[�I����
	EnemyManager::Instance().Clear();

	DamageTextManager::Instance().Clear();

	StageManager::Instance().Clear();

	LightManager::Instance().Clear();

	delete gauge;
}

// �X�V
void SceneContextBase::Update()
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
void SceneContextBase::Render()
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

		PlayerManager::Instance().Render();

		EnemyManager::Instance().Render();

		gfx.shadowBuffer->DeActivate();
	}


	// --- ImGui�̃f�o�b�O�K�p���� ---
	gfx.SetRasterizer(static_cast<RASTERIZER_STATE>(rasterizerSelect));

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

	PlayerManager::Instance().Render();

	EnemyManager::Instance().Render();

	//--- < ���X�^���C�U�̃o�C���h > ---
	gfx.SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_CULL_NONE);

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

	DrawGrid(20, 1.0f);

	LightManager::Instance().DrawDebugPrimitive();
	LightManager::Instance().DrawDebugGui();

	// --- �f�o�b�O�`�� ---
	DrawDebugGUI();

	DebugPrimitive::Instance().Render();
	LineRenderer::Instance().Render();
}

// �f�o�b�O�`��
void SceneContextBase::DrawDebugGUI()
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

	// --- Camera and Light ---
	if (ImGui::Begin("Light", nullptr, ImGuiWindowFlags_None))
	{
		// --- lightDirection ---
		{
			if (ImGui::CollapsingHeader("lightDirection", ImGuiTreeNodeFlags_DefaultOpen))
			{
				// �ʒu
				ImGui::DragFloat3("Direction", &lightDirection.x, 0.1f);
			}
		}
	}
	ImGui::End();

	// -- camera ---
	Camera::Instance().DrawDebugGui();

	ImGui::Begin("ContextBase");
	{
		ImGui::Checkbox("Plus", &contextAdd);
		ImGui::SameLine();
		if (ImGui::Button("Clear"))
		{
			contexts.clear();
		}
		ImGui::SliderFloat("Power", &contextPower, 0.0f, 1.0f);

		// �E�B���h�E���ŕ`����J�n
		ImGui::BeginChild("Player Direction", ImVec2(300, 300), false);

		ImVec2 p = ImGui::GetCursorScreenPos();
		ImVec2 size = { ImGui::GetWindowSize().x - 1, ImGui::GetWindowSize().y - 1 };
		ImGui::GetWindowDrawList()->AddLine(p, ImVec2(p.x + size.x, p.y), IM_COL32(255, 0, 0, 255));
		ImGui::GetWindowDrawList()->AddLine(p, ImVec2(p.x, p.y + size.y), IM_COL32(255, 0, 0, 255));
		ImGui::GetWindowDrawList()->AddLine(ImVec2(p.x + size.x, p.y), ImVec2(p.x + size.x, p.y + size.y), IM_COL32(255, 0, 0, 255));
		ImGui::GetWindowDrawList()->AddLine(ImVec2(p.x, p.y + size.y), ImVec2(p.x + size.x, p.y + size.y), IM_COL32(255, 0, 0, 255));


		// ���S����̐���`��
		ImVec2 center = ImVec2(p.x + 150, p.y + 150);
		float radius = 100.0f;

		// �~�̕`��
		ImGui::GetWindowDrawList()->AddCircle(center, radius, IM_COL32(255, 255, 255, 255), 0, 2.0f);

		// �}�E�X�̈ʒu���擾���ē_��`��
		ImVec2 mousePos = ImGui::GetMousePos();
		ImGui::GetWindowDrawList()->AddCircleFilled(mousePos, 5.0f, IM_COL32(255, 0, 0, 255));

		InputManager& input = InputManager::Instance();
		if (input.GetMousePressed(MOUSEBUTTON_STATE::leftButton) && input.GetKeyPress(DirectX::Keyboard::LeftShift))
		{
			Context context;
			context.plus = contextAdd;
			DirectX::XMVECTOR Pos = { mousePos.x - center.x, mousePos.y - center.y};
			DirectX::XMVECTOR NormalPos = DirectX::XMVector3Normalize(Pos);
			DirectX::XMStoreFloat2(&context.pos, NormalPos);
			context.pos = { mousePos.x, mousePos.y };
			context.power = contextPower;
			contexts.push_back(context);
		}



		for (float r = 0; r <= 360; r += 22.5)
		{
			float radian = DirectX::XMConvertToRadians(r);
			ImVec2 end = ImVec2(center.x + cosf(radian) * radius, center.y + sinf(radian) * radius);

			DirectX::XMVECTOR vec = { end.x - center.x, end.y - center.y };
			DirectX::XMVECTOR mouseVec = { mousePos.x - center.x, mousePos.y - center.y };
			DirectX::XMVECTOR normalVec = DirectX::XMVector3Normalize(vec);
			DirectX::XMVECTOR mouseNormalVec = DirectX::XMVector3Normalize(mouseVec);
			DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(normalVec, mouseNormalVec);

			float dot = 0.0f;
			DirectX::XMStoreFloat(&dot, Dot);
			dot = contextPower - fabsf(dot);
			
			for (auto context : contexts)
			{
				DirectX::XMFLOAT2 contextPos = { context.pos.x - center.x, context.pos.y - center.y };
				DirectX::XMVECTOR contextVector = DirectX::XMLoadFloat2(&contextPos);
				DirectX::XMVECTOR contextVectorN = DirectX::XMVector3Normalize(contextVector);
				DirectX::XMVECTOR ContextDot = DirectX::XMVector3Dot(normalVec, contextVectorN);
				float contextDot;
				DirectX::XMStoreFloat(&contextDot, ContextDot);
				contextDot *= context.power;
				if (contextDot < 0) contextDot = 0;

				dot += (context.plus) ? contextDot : -contextDot;

				DirectX::XMFLOAT4 color = (context.plus) ? DirectX::XMFLOAT4(0, 255, 0, 255) : DirectX::XMFLOAT4(255, 0, 0, 255);
				ImVec2 pos = { context.pos.x, context.pos.y };
				ImGui::GetWindowDrawList()->AddCircleFilled(pos, 5.0f, IM_COL32(color.x, color.y, color.z, color.w));

				if (dot < 0) dot = 0;
			}

			end = ImVec2(center.x + cosf(radian) * radius * dot, center.y + sinf(radian) * radius * dot);

			ImGui::GetWindowDrawList()->AddLine(center, end, IM_COL32(0, 255, 0, 255));
		}

		// �E�B���h�E���ł̕`����I��
		ImGui::EndChild();
	}
	ImGui::End();
}

// �G�l�~�[HP�Q�[�W�`��
void SceneContextBase::RenderEnemyGauge()
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
			//// �G��z�u
			//EnemyContextBaseSlime* slime = new EnemyContextBaseSlime();
			//slime->SetPosition(hit.position);
			//EnemyManager::Instance().Register(slime);

			StageManager& stageManager = StageManager::Instance();
			StageBox* stageBox = new_ StageBox();
			stageBox->SetPosition(hit.position);
			stageManager.Register(stageBox);

		}
	}
}


// �O���b�h�`��
void SceneContextBase::DrawGrid(int subdivisions, float scale)
{
	int numLines = (subdivisions + 1) * 2;
	int vertexCount = numLines * 2;

	float corner = 0.5f;
	float step = 1.0f / static_cast<float>(subdivisions);

	int index = 0;
	float s = -corner;

	const DirectX::XMFLOAT4 white = DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1);

	LineRenderer& lineRenderer = LineRenderer::Instance();
	// Create vertical lines
	float scaling = static_cast<float>(subdivisions) * scale;
	DirectX::XMMATRIX M = DirectX::XMMatrixScaling(scaling, scaling, scaling);
	DirectX::XMVECTOR V, P;
	DirectX::XMFLOAT3 position;
	for (int i = 0; i <= subdivisions; i++)
	{
		V = DirectX::XMVectorSet(s, 0, corner, 0);
		P = DirectX::XMVector3TransformCoord(V, M);
		DirectX::XMStoreFloat3(&position, P);
		lineRenderer.AddVertex(position, white);

		V = DirectX::XMVectorSet(s, 0, -corner, 0);
		P = DirectX::XMVector3TransformCoord(V, M);
		DirectX::XMStoreFloat3(&position, P);
		lineRenderer.AddVertex(position, white);

		s += step;
	}

	// Create horizontal lines
	s = -corner;
	for (int i = 0; i <= subdivisions; i++)
	{
		V = DirectX::XMVectorSet(corner, 0, s, 0);
		P = DirectX::XMVector3TransformCoord(V, M);
		DirectX::XMStoreFloat3(&position, P);
		lineRenderer.AddVertex(position, white);

		V = DirectX::XMVectorSet(-corner, 0, s, 0);
		P = DirectX::XMVector3TransformCoord(V, M);
		DirectX::XMStoreFloat3(&position, P);
		lineRenderer.AddVertex(position, white);

		s += step;
	}

	// X��
	{
		const DirectX::XMFLOAT4 red = DirectX::XMFLOAT4(1, 0, 0, 1);
		V = DirectX::XMVectorSet(0, 0, 0, 0);
		P = DirectX::XMVector3TransformCoord(V, M);
		DirectX::XMStoreFloat3(&position, P);
		lineRenderer.AddVertex(position, red);

		V = DirectX::XMVectorSet(corner, 0, 0, 0);
		P = DirectX::XMVector3TransformCoord(V, M);
		DirectX::XMStoreFloat3(&position, P);
		lineRenderer.AddVertex(position, red);
	}

	// Y��
	{
		const DirectX::XMFLOAT4 green = DirectX::XMFLOAT4(0, 1, 0, 1);
		V = DirectX::XMVectorSet(0, 0, 0, 0);
		P = DirectX::XMVector3TransformCoord(V, M);
		DirectX::XMStoreFloat3(&position, P);
		lineRenderer.AddVertex(position, green);

		V = DirectX::XMVectorSet(0, corner, 0, 0);
		P = DirectX::XMVector3TransformCoord(V, M);
		DirectX::XMStoreFloat3(&position, P);
		lineRenderer.AddVertex(position, green);
	}

	// Z��
	{
		const DirectX::XMFLOAT4 blue = DirectX::XMFLOAT4(0, 0, 1, 1);
		V = DirectX::XMVectorSet(0, 0, 0, 0);
		P = DirectX::XMVector3TransformCoord(V, M);
		DirectX::XMStoreFloat3(&position, P);
		lineRenderer.AddVertex(position, blue);

		V = DirectX::XMVectorSet(0, 0, corner, 0);
		P = DirectX::XMVector3TransformCoord(V, M);
		DirectX::XMStoreFloat3(&position, P);
		lineRenderer.AddVertex(position, blue);
	}
}
