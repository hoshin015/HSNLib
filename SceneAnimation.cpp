#include <tchar.h>
#include <fstream>
#include <filesystem>
#include "SceneAnimation.h"
#include "SceneManager.h"
#include "Library/Framework.h"
#include "Library/Timer.h"
#include "Library/ImGui/Include/imgui.h"
#include "Library/Graphics/Graphics.h"
#include "Library/3D/Camera.h"
#include "Library/3D/DebugPrimitive.h"
#include "Library/ImGui/Include/imgui_impl_dx11.h"
#include "Library/ImGui/Include/imgui_impl_win32.h"
#include "Library/Effekseer/Effect.h"
#include "Library/Effekseer/EffectManager.h"
#include "Library/3D/LineRenderer.h"

// ������
void SceneAnimation::Initialize()
{
	skyMap = std::make_unique<SkyMap>(L"Data/Texture/kloppenheim_05_puresky_4k.hdr");

	// --- �J���������ݒ� ---
	Camera::Instance().SetLookAt(
		DirectX::XMFLOAT3(0, 5, 20),		// �J�������W
		DirectX::XMFLOAT3(-30, 0, -30),		// �^�[�Q�b�g(�ݒ肵�Ă��Ӗ��Ȃ�)
		DirectX::XMFLOAT3(0, 1, 0)			// ������x�N�g��
	);
	Camera::Instance().SetAngle({ DirectX::XMConvertToRadians(15),DirectX::XMConvertToRadians(180), 0 });

	Camera::Instance().cameraType = Camera::CAMERA::ANIMATION_EDITOR;

	// --- �V�[�N�G���X�ϐ������ݒ� ---
	mySequence.mFrameMin = 0;
	mySequence.mFrameMax = 60;
}

// �I����
void SceneAnimation::Finalize()
{
	ModelClear();
}

// �X�V
void SceneAnimation::Update()
{
	// �J�����R���g���[���[�X�V����
	DirectX::XMFLOAT3 target = modelPosition;
	Camera::Instance().SetTarget(target);
	Camera::Instance().Update();

	if (mySequence.myItems.size() == 0)
	{
		mySequence.Add("", 0, -10, -10);
	}

	// --- �A�j���[�V�������� ---
	if (model)
	{
		if (model->animationClips.size() > 0)
		{
			// �Đ���
			if (isPlay)
			{
				currentFrameFloat += Timer::Instance().DeltaTime() * 30 * timeScale;
				if (currentFrameFloat > mySequence.mFrameMax)
				{
					if (isLoop)
					{
						ClearEffectFlag();
						ClearSEFlag();
						//currentFrameFloat -= mySequence.mFrameMax;
						currentFrameFloat = 0;
					}
					else
					{
						currentFrameFloat = mySequence.mFrameMax;
						isPlay = false;
					}
				}
				currentFrameInt = static_cast<int>(currentFrameFloat);
			}
			// ��~��
			else
			{
				ClearEffectFlag();
				ClearSEFlag();
				currentFrameFloat = currentFrameInt;
			}

			// �G�t�F�N�g����
			if (isPlay)
			{
				int effectCount = model->animationClips.at(animationClipIndex).animEffects.size();

				for (int i = 0; i < effectCount; i++)
				{
					// ���ɍĐ�����Ă���Ȃ�Đ����Ȃ�
					if (model->animationClips.at(animationClipIndex).animEffects.at(i).isPlay) continue;

					int startFrame = model->animationClips.at(animationClipIndex).animEffects.at(i).startFrame;
					int endFrame = model->animationClips.at(animationClipIndex).animEffects.at(i).endFrame;

					//if (currentFrameInt < startFrame || currentFrameInt > endFrame) continue;
					if (currentFrameInt != startFrame) continue;

					model->animationClips.at(animationClipIndex).animEffects.at(i).isPlay = true;

					EffectType type = model->animationClips.at(animationClipIndex).animEffects.at(i).effectType;
					float scale = model->animationClips.at(animationClipIndex).animEffects.at(i).scale;

					DirectX::XMFLOAT3 position = model->animationClips.at(animationClipIndex).animEffects.at(i).position;
					DirectX::XMFLOAT3 angle = model->animationClips.at(animationClipIndex).animEffects.at(i).angle;

					Effect::Instance().Play(type, position, angle, scale);
				}
			}

			// ���ʉ�����
			if (isPlay)
			{
				int seCount = model->animationClips.at(animationClipIndex).animSEs.size();

				for (int i = 0; i < seCount; i++)
				{
					// ���ɍĐ�����Ă���Ȃ�Đ����Ȃ�
					if (model->animationClips.at(animationClipIndex).animSEs.at(i).isPlay) continue;

					int startFrame = model->animationClips.at(animationClipIndex).animSEs.at(i).startFrame;
					int endFrame = model->animationClips.at(animationClipIndex).animSEs.at(i).endFrame;

					//if (currentFrameInt < startFrame || currentFrameInt > endFrame) continue;
					if (currentFrameInt != startFrame) continue;

					model->animationClips.at(animationClipIndex).animSEs.at(i).isPlay = true;

					MUSIC_LABEL type = model->animationClips.at(animationClipIndex).animSEs.at(i).musicType;

					AudioManager::Instance().PlayMusic(static_cast<int>(type));
				}
			}
		}
	}

}

// �`��
void SceneAnimation::Render()
{
	// --- Graphics �擾 ---
	Graphics& gfx = Graphics::Instance();

	skyMap->Render();

	gfx.SetDepthStencil(DEPTHSTENCIL_STATE::ZT_ON_ZW_ON);

	Graphics::SceneConstants data{};
	XMMATRIX viewProjection = XMLoadFloat4x4(&Camera::Instance().GetView()) * XMLoadFloat4x4(&Camera::Instance().GetProjection());
	DirectX::XMStoreFloat4x4(&data.viewProjection, viewProjection);	// �r���[�@�v���W�F�N�V�����@�ϊ��s����܂Ƃ߂�
	data.directionalLightData.direction = { lightDirection.x, lightDirection.y, lightDirection.z, 0 };
	data.directionalLightData.color = lightColor;
	data.ambientLightColor = ambientLightColor;
	data.cameraPosition = { Camera::Instance().GetEye().x, Camera::Instance().GetEye().y, Camera::Instance().GetEye().z, 0 };
	gfx.deviceContext->UpdateSubresource(gfx.constantBuffer.Get(), 0, 0, &data, 0, 0);
	gfx.deviceContext->VSSetConstantBuffers(1, 1, gfx.constantBuffer.GetAddressOf());
	gfx.deviceContext->PSSetConstantBuffers(1, 1, gfx.constantBuffer.GetAddressOf());

	// --- ���f���`�� ---
	if (model)
	{
		// rasterizer �̐ݒ�
		gfx.SetRasterizer(RASTERIZER_STATE::CLOCK_TRUE_SOLID);

		// �T�C�Y�̏C��
		const float scaleFactor = model->scaleFactors[model->fbxUnit];
		DirectX::XMMATRIX C = DirectX::XMLoadFloat4x4(&model->coordinateSystemTransform[model->coordinateSystemIndex]) * DirectX::XMMatrixScaling(scaleFactor, scaleFactor, scaleFactor);

		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(modelScale, modelScale, modelScale);
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(XMConvertToRadians(modelAngle.x), XMConvertToRadians(modelAngle.y), XMConvertToRadians(modelAngle.z));
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(modelPosition.x, modelPosition.y, modelPosition.z);

		DirectX::XMFLOAT4X4 world;
		DirectX::XMStoreFloat4x4(&world, C * S * R * T);

		// �A�j���[�V��������
		if (model->animationClips.size() > 0)
		{
			Animation& animation = model->animationClips.at(animationClipIndex);

			Animation::KeyFrame& keyFrame = animation.sequence.at(currentFrameInt);

			model->Render(world, { 1,1,1,modelAlpha }, &keyFrame);
		}
		else
		{
			model->Render(world, { 1,1,1,modelAlpha }, 0);
		}
	}

	//--- < ���X�^���C�U�̃o�C���h > ---
	gfx.SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_CULL_NONE);

	EffectManager::Instance().Render(Camera::Instance().GetView(), Camera::Instance().GetProjection());

	// --- �R���W�����`�� ---
	if (model && model->animationClips.size() > 0)
	{
		// sphere
		int collisionCount = model->animationClips.at(animationClipIndex).spheres.size();

		for (int i = 0; i < collisionCount; i++)
		{
			int startFrame = model->animationClips.at(animationClipIndex).spheres.at(i).startFrame;
			int endFrame = model->animationClips.at(animationClipIndex).spheres.at(i).endFrame;

			if (currentFrameInt < startFrame || currentFrameInt > endFrame) continue;

			float radius = model->animationClips.at(animationClipIndex).spheres.at(i).radius;
			DirectX::XMFLOAT4 color = model->animationClips.at(animationClipIndex).spheres.at(i).color;

			DirectX::XMFLOAT3 position = model->animationClips.at(animationClipIndex).spheres.at(i).position;
			if (model->animationClips.at(animationClipIndex).spheres.at(i).bindBoneName != "")
			{
				Animation::KeyFrame& keyframe = model->animationClips.at(animationClipIndex).sequence.at(currentFrameInt);

				int nodeCount = keyframe.nodes.size();
				for (int nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
				{
					Animation::KeyFrame::Node& node = keyframe.nodes.at(nodeIndex);

					// �{�[���̃m�[�h�Ɠ������O�Ȃ珈������
					if (node.name != model->animationClips.at(animationClipIndex).spheres.at(i).bindBoneName) continue;

					DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&node.globalTransform);

					const float scaleFactor = model->scaleFactors[model->fbxUnit];
					DirectX::XMMATRIX C = DirectX::XMLoadFloat4x4(&model->coordinateSystemTransform[model->coordinateSystemIndex]) * DirectX::XMMatrixScaling(scaleFactor, scaleFactor, scaleFactor);

					DirectX::XMMATRIX W = M * C;
					DirectX::XMFLOAT4X4 w;
					DirectX::XMStoreFloat4x4(&w, W);

					DirectX::XMFLOAT3 position = { w._41, w._42, w._43 };

					model->animationClips.at(animationClipIndex).spheres.at(i).position.x = position.x;
					model->animationClips.at(animationClipIndex).spheres.at(i).position.y = position.y;
					model->animationClips.at(animationClipIndex).spheres.at(i).position.z = position.z;
				}
			}
			
			DebugPrimitive::Instance().AddSphere(position, radius, color);
		}
	}

	DrawGrid(10, 1.0f);
	
	// --- �f�o�b�O�`�� ---
	DrawDebugGUI();

	DebugPrimitive::Instance().Render();
	LineRenderer::Instance().Render();
}

// �O���b�h�`��
void SceneAnimation::DrawGrid(int subdivisions, float scale)
{
	int numLines = (subdivisions + 1) * 2;
	int vertexCount = numLines * 2;

	float corner = 0.5f;
	float step = 1.0f / static_cast<float>(subdivisions);

	int index = 0;
	float s = -corner;

	const DirectX::XMFLOAT4 white = DirectX::XMFLOAT4(1, 1, 1, 1);

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


// �f�o�b�O�`��
void SceneAnimation::DrawDebugGUI()
{
	//----------------------------------------------------------
	// ���j���[
	//----------------------------------------------------------
	//if (ImGui::BeginMainMenuBar()) {
	//	if (ImGui::BeginMenu("File")) {
	//		if (ImGui::BeginMenu("Scene")) {
	//			if (ImGui::MenuItem("Title"))
	//			{
	//				SceneManager::Instance().ChangeScene(new SceneTitle);
	//			}
	//			if (ImGui::MenuItem("Game"))
	//			{
	//				SceneManager::Instance().ChangeScene(new SceneLoading(new SceneGame));
	//			}
	//			if (ImGui::MenuItem("Animation"))
	//			{
	//				SceneManager::Instance().ChangeScene(new SceneAnimation);
	//			}
	//			ImGui::EndMenu();
	//		}
	//		ImGui::EndMenu();
	//	}
	//	ImGui::EndMainMenuBar();
	//}
	ImGui::Begin(u8"�^�C�g���J�ڗp");
	if (ImGui::Button(u8"�^�C�g��"))
	{
		SceneManager::Instance().ChangeScene(new SceneTitle);
	}
	ImGui::End();

	//----------------------------------------------------------
	//  �J���� 
	//----------------------------------------------------------
	Camera::Instance().DrawDebugGui();

	//----------------------------------------------------------
	//  ���[�h
	//----------------------------------------------------------
	ImGui::Begin(u8"���[�h");

	// --- FbxPath ---
	if (ImGui::Button(u8"FbxPath �ݒ�"))
	{
		OPENFILENAME ofn;       // �t�@�C���I��p�̍\����
		TCHAR szFile[260] = { 0 };  // �t�@�C���p�X���i�[����o�b�t�@

		// �\���̂̏�����
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lpstrFilter = _TEXT("fbx�t�@�C��(*.fbx)\0*.fbx\0") _TEXT("�S�Ẵt�@�C��(*.*)\0*.*\0");
		ofn.lStructSize = sizeof(ofn);
		ofn.lpstrFile = szFile;
		ofn.lpstrFile[0] = '\0';
		ofn.nMaxFile = sizeof(szFile);
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		GetOpenFileName(&ofn);

		// --- filesystem �ɂ��path�ϊ� ---

		std::filesystem::path selectedPath(szFile);
		// ���݂̃f�B���N�g���̃p�X���擾
		std::filesystem::path currentPath = std::filesystem::current_path();
		// �I�����ꂽ�t�@�C���ւ̑��΃p�X���擾
		std::filesystem::path relativePath = selectedPath.lexically_relative(currentPath);
		fbxPath = relativePath.string();
	}
	ImGui::InputText("FbxPath", (char*)fbxPath.c_str(), sizeof(fbxPath));

	// --- model ���� ---
	if (ImGui::Button(u8"���f���ǂݍ���"))
	{
		ModelClear();
		model = new SkinnedMesh(fbxPath.c_str(), triangulate);

		animationClipIndex = 0;

		selectedEntry = -1;
		mySequence.myItems.clear();
		mySequence.selectItemNum = 0;

		// �A�C�e����ǉ�
		if (model->animationClips.size() > 0)
		{
			int sphereCount = model->animationClips.at(animationClipIndex).spheres.size();
			for (auto sphere : model->animationClips.at(animationClipIndex).spheres)
			{
				mySequence.Add(sphere.name.c_str(), static_cast<int>(SequencerItemType::Sphere), sphere.startFrame, sphere.endFrame);
			}
			int effectCount = model->animationClips.at(animationClipIndex).animEffects.size();
			for (auto effect : model->animationClips.at(animationClipIndex).animEffects)
			{
				mySequence.Add(effect.name.c_str(), static_cast<int>(SequencerItemType::Effect), effect.startFrame, effect.endFrame);
			}
			int seCount = model->animationClips.at(animationClipIndex).animSEs.size();
			for (auto se : model->animationClips.at(animationClipIndex).animSEs)
			{
				mySequence.Add(se.name.c_str(), static_cast<int>(SequencerItemType::SE), se.startFrame, se.endFrame);
			}

		}
		
		// �����A�C�e�����Ȃ����
		if (mySequence.myItems.size() == 0)
		{
			mySequence.Add("", 0, -10, -10);
		}

		if (model->animationClips.size() > 0)
		{
			mySequence.mFrameMax = model->animationClips.at(0).sequence.size() - 1;
		}
	}

	if (ImGui::Button(u8".model �Đ���"))
	{
		std::string modelPathStr = model->fbxPath;
		std::filesystem::path modelPath(modelPathStr);
		modelPath.replace_extension(".model");
		std::filesystem::remove(modelPath);

		ModelClear();
		model = new SkinnedMesh(fbxPath.c_str(), triangulate);

		animationClipIndex = 0;

		if (model->animationClips.size() > 0)
		{
			mySequence.mFrameMax = model->animationClips.at(0).sequence.size() - 1;
		}
	}
	ImGui::SameLine();
	ImGui::Checkbox(u8"�O�p�`��", &triangulate);

	if (ImGui::Button(u8"�A�j���[�V�����ǉ�"))
	{
		if (model)
		{
			model->AppendAnimations(fbxPath.c_str(), 0);
		}
	}
	ImGui::End();

	//----------------------------------------------------------
	// ���f��
	//----------------------------------------------------------
	ImGui::Begin(u8"���f��");

	if (model)
	{
		const char* coordinateSystemTransformName[] = { "RHS Y - UP", "LHS Y - UP", "RHS Z - UP", "LHS Z - UP"};
		if (ImGui::BeginCombo(u8"������", coordinateSystemTransformName[model->coordinateSystemIndex]))
		{
			for (int i = 0; i < IM_ARRAYSIZE(coordinateSystemTransformName); i++)
			{
				const bool isSelected = (model->coordinateSystemIndex == i);
				if (ImGui::Selectable(coordinateSystemTransformName[i], isSelected))
				{
					// �l�̕ύX�Ɠ����� cereal �����o��
					model->SetCoordinateSystemTransform(static_cast<CoordinateSystemTransform>(i));
				}
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		const char* scaleFactorName[] = { u8"�Z���`���[�g��", u8"���[�g��" };
		if (ImGui::BeginCombo(u8"���f���P��", scaleFactorName[model->fbxUnit]))
		{
			for (int i = 0; i < IM_ARRAYSIZE(scaleFactorName); i++)
			{
				const bool isSelected = (model->fbxUnit == i);
				if (ImGui::Selectable(scaleFactorName[i], isSelected))
				{
					// �l�̕ύX�Ɠ����� cereal �����o��
					model->SetFbxUnit(static_cast<FbxUnit>(i));
				}
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}

		// �g�����X�t�H�[��
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// �ʒu
			ImGui::DragFloat3("Position", &modelPosition.x, 0.1f);
			// ��]
			ImGui::DragFloat3("Angle", &modelAngle.x);
			// �X�P�[��
			ImGui::DragFloat("Scale", &modelScale, 0.1f);

			ImGui::DragFloat("Alpha", &modelAlpha, 0.1f);
		}
		ImGui::InputInt("Clip", &animationClipIndex);
	}

	ImGui::End();

	//----------------------------------------------------------
	// �A�j���[�V����
	//----------------------------------------------------------
	ImGui::Begin(u8"�A�j���[�V����");

	if (model)
	{
		if (ImGui::Button(u8".anim�����o��"))
		{
			// --- �A�j���[�V�����f�B���N�g������x�폜���Ă��琶�� ---
			std::string animPath = model->parentPath + "/Anim/";

			std::filesystem::remove_all(animPath);
			std::filesystem::create_directory(animPath);

			int animCount = model->animationClips.size();
			for (int animIndex = 0; animIndex < animCount; animIndex++)
			{
				// .anim �̖��O�ύX
				std::string newFilename = model->parentPath + "/Anim/" + model->animationClips.at(animIndex).name + ".anim";
				std::filesystem::path newFilePath(newFilename);

				// �o��
#if 1
				std::ofstream ofs(newFilename.c_str(), std::ios::binary);
				cereal::BinaryOutputArchive  serialization(ofs);
#else
				std::ofstream ofs(newFilename.c_str(), std::ios::out);
				cereal::JSONOutputArchive  serialization(ofs);
#endif
				serialization(model->animationClips.at(animIndex));
			}
			
		}

		int animCount = model->animationClips.size();
		for (int animIndex = 0; animIndex < animCount; animIndex++)
		{
			if(ImGui::RadioButton((model->animationClips.at(animIndex).name).c_str(), &animationClipIndex, animIndex))
			{
				mySequence.mFrameMax = model->animationClips.at(animIndex).sequence.size() - 1;

				currentFrameInt = 0;
				currentFrameFloat = 0.0f;

				selectedEntry = -1;
				// sequence���̃A�C�e�����N���A
				mySequence.myItems.clear();
				// ��ԏ�̃A�C�e����I����Ԃ�
				mySequence.selectItemNum = 0;

				// �A�C�e����ǉ�
				int sphereCount = model->animationClips.at(animIndex).spheres.size();
				for (auto sphere : model->animationClips.at(animIndex).spheres)
				{
					mySequence.Add(sphere.name.c_str(), static_cast<int>(SequencerItemType::Sphere), sphere.startFrame, sphere.endFrame);
				}
				int effectCount = model->animationClips.at(animIndex).animEffects.size();
				for (auto effect : model->animationClips.at(animIndex).animEffects)
				{
					mySequence.Add(effect.name.c_str(), static_cast<int>(SequencerItemType::Effect), effect.startFrame, effect.endFrame);
				}
				int seCount = model->animationClips.at(animIndex).animSEs.size();
				for (auto se : model->animationClips.at(animIndex).animSEs)
				{
					mySequence.Add(se.name.c_str(), static_cast<int>(SequencerItemType::SE), se.startFrame, se.endFrame);
				}


				// �����A�C�e�����Ȃ����
				if (mySequence.myItems.size() == 0)
				{
					mySequence.Add("", 0, -10, -10);
				}
			}
		}
	}
	ImGui::End();

	//----------------------------------------------------------
	// �{�[��
	//----------------------------------------------------------
	ImGui::Begin(u8"�{�[��");

	if (model)
	{
		if (model)
		{
			static float boneRadius = 0.1f;
			ImGui::DragFloat(u8"�{�[�����a", &boneRadius, 0.01f);


			// �A�j���[�V�����������Ă���ꍇ
			if (model->animationClips.size() > 0)
			{
				Animation::KeyFrame& keyframe = model->animationClips.at(animationClipIndex).sequence.at(currentFrameInt);

				int nodeCount = keyframe.nodes.size();
				for (int nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
				{
					Animation::KeyFrame::Node& node = keyframe.nodes.at(nodeIndex);

					DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&node.globalTransform);

					const float scaleFactor = model->scaleFactors[model->fbxUnit];
					DirectX::XMMATRIX C = DirectX::XMLoadFloat4x4(&model->coordinateSystemTransform[model->coordinateSystemIndex]) * DirectX::XMMatrixScaling(scaleFactor, scaleFactor, scaleFactor);

					DirectX::XMMATRIX W = M * C;
					DirectX::XMFLOAT4X4 w;
					DirectX::XMStoreFloat4x4(&w, W);

					DirectX::XMFLOAT3 position = { w._41, w._42, w._43 };

					if (ImGui::CollapsingHeader(node.name.c_str()))
					{
						ImGui::DragFloat3("POSITION", &position.x, 0.01f);
						DebugPrimitive::Instance().AddSphere(position, boneRadius, { 1,0,0,1 });
					}
				}
			}
		}
	}

	ImGui::End();

	//----------------------------------------------------------
	// �{�[��
	//----------------------------------------------------------
	ImGui::Begin(u8"�{�[�������蔻��");
	{
		if (model && model->animationClips.size() > 0)
		{
			if (ImGui::Button(u8"�����o��"))
			{
				std::filesystem::path newFilePath(model->fbxPath);
				newFilePath.replace_extension(".model");

				// �o��
#if 0
				std::ofstream ofs(newFilePath.c_str(), std::ios::binary);
				cereal::BinaryOutputArchive serialization(ofs);
#else
				std::ofstream ofs(newFilePath.c_str(), std::ios::out);
				cereal::JSONOutputArchive  serialization(ofs);
#endif
				serialization(model->sceneView, model->coordinateSystemIndex, model->fbxUnit, model->skeletonSpheres);
			}

			static std::string boneName = "";
			static float boneRadius = 1.0f;

			ImGui::InputText(u8"���O", (char*)boneName.c_str(), sizeof(boneName));
			ImGui::DragFloat(u8"���a", &boneRadius);

			if (ImGui::Button(u8"�{�[�������蔻��ǉ�"))
			{
				SkeletonSphere skeletonSphere;
				skeletonSphere.name = boneName.c_str();
				skeletonSphere.radius = boneRadius;
				model->skeletonSpheres.push_back(skeletonSphere);
			}

			std::vector<int> deleteBonesIndex;
			int boneSphereCount = model->skeletonSpheres.size();
			for (int boneSphereIndex = 0; boneSphereIndex < boneSphereCount; boneSphereIndex++)
			{
				if (ImGui::CollapsingHeader(model->skeletonSpheres.at(boneSphereIndex).name.c_str()))
				{
					SkeletonSphere& seletonSphere = model->skeletonSpheres.at(boneSphereIndex);
					ImGui::Text(("name : " + seletonSphere.name).c_str());
					ImGui::DragFloat(("radius - " + std::to_string(boneSphereIndex)).c_str(), &seletonSphere.radius, 0.01f);

					ImGui::SameLine();
					if (ImGui::Button(("delete - " + std::to_string(boneSphereIndex)).c_str()))
					{
						deleteBonesIndex.push_back(boneSphereIndex);
					}
				}

				float radius = model->skeletonSpheres.at(boneSphereIndex).radius;

				DirectX::XMFLOAT3 position = { 0,0,0 };
				if (model->skeletonSpheres.at(boneSphereIndex).name != "")
				{
					Animation::KeyFrame& keyframe = model->animationClips.at(animationClipIndex).sequence.at(currentFrameInt);

					int nodeCount = keyframe.nodes.size();
					for (int nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
					{
						Animation::KeyFrame::Node& node = keyframe.nodes.at(nodeIndex);

						// �{�[���̃m�[�h�Ɠ������O�Ȃ珈������
						if (node.name != model->skeletonSpheres.at(boneSphereIndex).name) continue;

						DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&node.globalTransform);

						const float scaleFactor = model->scaleFactors[model->fbxUnit];
						DirectX::XMMATRIX C = DirectX::XMLoadFloat4x4(&model->coordinateSystemTransform[model->coordinateSystemIndex]) * DirectX::XMMatrixScaling(scaleFactor, scaleFactor, scaleFactor);

						DirectX::XMMATRIX W = M * C;
						DirectX::XMFLOAT4X4 w;
						DirectX::XMStoreFloat4x4(&w, W);

						position = { w._41, w._42, w._43 };
					}
				}

				DebugPrimitive::Instance().AddSphere(position, radius, { 0,0,1,1 });

			}
			// �v�f�̍폜
			for (auto index : deleteBonesIndex)
			{
				model->skeletonSpheres.erase(model->skeletonSpheres.begin() + index);
			}


			
		}
		
	}
	ImGui::End();



	//----------------------------------------------------------
	// �C���X�y�N�^�[ 
	//----------------------------------------------------------
	{
		// --- window�t���O�ݒ� ---
		static ImGuiWindowFlags windowFlags =
			//ImGuiWindowFlags_MenuBar |		// �����L���ɂ���ƃ��j���[�o�[��ǉ��ł���
			ImGuiDockNodeFlags_PassthruCentralNode |
			//ImGuiWindowFlags_NoDocking |
			//ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;


		// --- dockNode�t���O�ݒ� ---
		const ImGuiDockNodeFlags dockFlags = ImGuiDockNodeFlags_PassthruCentralNode;

		// --- ImGui�̕\���ݒ� ---
		ImGui::SetNextWindowBgAlpha(0.0f);					// �w�i�A���t�@�̐ݒ�

		// --- DockSpace�̎��̓X�^�C���̐ݒ�? ---
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		// --- DockSpace�̍쐬 ---
		ImGui::Begin(u8"�C���X�y�N�^�[", NULL, windowFlags);
		ImGui::PopStyleVar(3);	// ���̓X�^�C���̓K�p?

		ImGuiID dockSpaceID = ImGui::GetID("DockInspector");
		ImGui::DockSpace(dockSpaceID, ImVec2(0.f, 0.f), dockFlags);

		ImGui::End();
	}

	//----------------------------------------------------------
	// �A�j���[�V������
	//----------------------------------------------------------
	ImGui::SetNextWindowBgAlpha(0.0f);					// �w�i�A���t�@�̐ݒ�
	ImGui::Begin(u8"�A�j���[�V�������O");
	ImGui::PushItemWidth(150);
	if (model && model->animationClips.size() > 0)
	{
		ImGui::InputText(u8"���O", (char*)model->animationClips.at(animationClipIndex).name.c_str(), sizeof(model->animationClips.at(animationClipIndex).name));
	}
	else
	{
		std::string s;
		ImGui::InputText(u8"���O", (char*)s.c_str(), sizeof(s));
	}
	ImGui::End();

	//----------------------------------------------------------
	// �A�j���[�V�������ύX
	//----------------------------------------------------------
	ImGui::SetNextWindowBgAlpha(0.0f);					// �w�i�A���t�@�̐ݒ�
	ImGui::Begin(u8"�A�j���[�V�����ύX");
	{
		if (model)
		{
			static std::string afterAnimationName = "";
			ImGui::InputText(u8"�ύX�㖼�O", (char*)afterAnimationName.c_str(), sizeof(afterAnimationName));

			if (ImGui::Button(u8"���O�ύX"))
			{
				// ��anim�t�@�C���̍폜
				std::string deleteFilename = model->parentPath + "/Anim/" + model->animationClips.at(animationClipIndex).name + ".anim";
				std::filesystem::remove(deleteFilename);

				// ��������̖��O�ύX
				model->animationClips.at(animationClipIndex).name = afterAnimationName.c_str();
				afterAnimationName = "";

				// .anim �̖��O�ύX
				std::string newFilename = model->parentPath + "/Anim/" + model->animationClips.at(animationClipIndex).name + ".anim";
				std::filesystem::path newFilePath(newFilename);

				// �o��
				std::ofstream ofs(newFilename.c_str(), std::ios::out);
				cereal::JSONOutputArchive  serialization(ofs);
				serialization(model->animationClips.at(animationClipIndex));
			}
			ImGui::SameLine();
			if (ImGui::Button(u8"�A�j���[�V�����폜"))
			{
				// �폜
				std::string deleteFilename = model->parentPath + "/Anim/" + model->animationClips.at(animationClipIndex).name + ".anim";
				std::filesystem::remove(deleteFilename);

				model->animationClips.erase(model->animationClips.begin() + animationClipIndex);

				// �폜��
				animationClipIndex = 0;		// �I�𒆂̃A�j���[�V�������ŏ��̃A�j���[�V�����ɂ���
				currentFrameInt = 0;
				currentFrameFloat = 0.0f;
				mySequence.mFrameMax = model->animationClips.at(animationClipIndex).sequence.size() - 1;
			}
		}
		else
		{
			ImGui::Text("Animation is not found.");
		}

	}
	ImGui::End();

	//----------------------------------------------------------
	// �V�K�^�C�����C���A�C�e��
	//----------------------------------------------------------
	ImGui::SetNextWindowBgAlpha(0.0f);					// �w�i�A���t�@�̐ݒ�
	static int selectSequencerItemTypeName;
	ImGui::Begin(u8"�V�K�^�C�����C���A�C�e��");
	{
		ImGui::PushItemWidth(150);
		ImGui::InputText(u8"���O", (char*)newSequenceName.c_str(), sizeof(newSequenceName));
		{
			ImGui::PushItemWidth(150);
			if (ImGui::BeginCombo(u8"�^�C�v", SequencerItemTypeNames[selectSequencerItemTypeName]))
			{
				for (int i = 0; i < IM_ARRAYSIZE(SequencerItemTypeNames); i++)
				{
					const bool isSelected = (selectSequencerItemTypeName == i);
					if (ImGui::Selectable(SequencerItemTypeNames[i], isSelected))
					{
						selectSequencerItemTypeName = i;
					}
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
		}
		ImGui::PopItemWidth();

		if (ImGui::Button(u8"�쐬"))
		{
			if (model)
			{
				// emptyItem �̍폜
				if (mySequence.myItems.size() == 1)
				{
					if (mySequence.myItems.at(0).mType == static_cast<int>(SequencerItemType::EMPTY))
					{
						mySequence.myItems.clear();
					}
				}

				if (selectSequencerItemTypeName == static_cast<int>(SequencerItemType::Sphere))
				{
					CollisionSphere collision;
					collision.name = newSequenceName;
					collision.startFrame = 0;
					collision.endFrame = 10;
					collision.radius = 1.0f;
					model->animationClips.at(animationClipIndex).spheres.push_back(collision);

					mySequence.Add(newSequenceName, selectSequencerItemTypeName, 0, 10);
				}
				if (selectSequencerItemTypeName == static_cast<int>(SequencerItemType::Effect))
				{
					AnimEffect animEffect;
					animEffect.name = newSequenceName;
					animEffect.startFrame = 0;
					animEffect.endFrame = 10;
					animEffect.scale = 1.0f;
					model->animationClips.at(animationClipIndex).animEffects.push_back(animEffect);

					mySequence.Add(newSequenceName, selectSequencerItemTypeName, 0, 10);
				}
				if (selectSequencerItemTypeName == static_cast<int>(SequencerItemType::SE))
				{
					AnimSE animSE;
					animSE.name = newSequenceName;
					animSE.startFrame = 0;
					animSE.endFrame = 10;
					model->animationClips.at(animationClipIndex).animSEs.push_back(animSE);

					mySequence.Add(newSequenceName, selectSequencerItemTypeName, 0, 10);
				}
			}
		}
	}
	ImGui::End();

	//----------------------------------------------------------
	// �I�𒆃^�C�����C���A�C�e��
	//----------------------------------------------------------
	ImGui::SetNextWindowBgAlpha(0.0f);					// �w�i�A���t�@�̐ݒ�
	ImGui::Begin(u8"�I�𒆃^�C�����C���A�C�e��");
	{
		if (model)
		{
			if(mySequence.selectItemNum != -1)
			{
				bool isSkip = false;

				// ����
				if (ImGui::Button(u8"����"))
				{
					if (mySequence.myItems.at(mySequence.selectItemNum).mType == static_cast<int>(SequencerItemType::Sphere))
					{
						int index = mySequence.myItems.at(mySequence.selectItemNum).mTypeIndex;

						CollisionSphere collision = model->animationClips.at(animationClipIndex).spheres.at(index);
						model->animationClips.at(animationClipIndex).spheres.push_back(collision);

						mySequence.Add(collision.name, static_cast<int>(SequencerItemType::Sphere), collision.startFrame, collision.endFrame);

					}
					if (mySequence.myItems.at(mySequence.selectItemNum).mType == static_cast<int>(SequencerItemType::Effect))
					{
						int index = mySequence.myItems.at(mySequence.selectItemNum).mTypeIndex;

						AnimEffect animEffect = model->animationClips.at(animationClipIndex).animEffects.at(index);
						model->animationClips.at(animationClipIndex).animEffects.push_back(animEffect);

						mySequence.Add(animEffect.name, static_cast<int>(SequencerItemType::Effect), animEffect.startFrame, animEffect.endFrame);
					}
				}

				// �폜
				ImGui::SameLine();
				if (ImGui::Button(u8"�폜"))
				{
					isSkip = true;

					// ���f���������Ă�A�C�e�����폜
					if (mySequence.myItems.at(mySequence.selectItemNum).mType == static_cast<int>(SequencerItemType::Sphere))
					{
						int index = mySequence.myItems.at(mySequence.selectItemNum).mTypeIndex;
						model->animationClips.at(animationClipIndex).spheres.erase(model->animationClips.at(animationClipIndex).spheres.begin() + index);
					}
					if (mySequence.myItems.at(mySequence.selectItemNum).mType == static_cast<int>(SequencerItemType::Effect))
					{
						int index = mySequence.myItems.at(mySequence.selectItemNum).mTypeIndex;
						model->animationClips.at(animationClipIndex).animEffects.erase(model->animationClips.at(animationClipIndex).animEffects.begin() + index);
					}
					if (mySequence.myItems.at(mySequence.selectItemNum).mType == static_cast<int>(SequencerItemType::SE))
					{
						int index = mySequence.myItems.at(mySequence.selectItemNum).mTypeIndex;
						model->animationClips.at(animationClipIndex).animSEs.erase(model->animationClips.at(animationClipIndex).animSEs.begin() + index);
					}

					// mySequence ���̃A�C�e�����폜
					mySequence.myItems.erase(mySequence.myItems.begin() + mySequence.selectItemNum);

					// �폜������ɑI�𒆂̃A�C�e�����O��
					mySequence.selectItemNum = 0;
					if (mySequence.myItems.size() == 0)
					{
						mySequence.Add("", 0, -10, -10);
					}

					selectedEntry = -1;
				}

				

				std::string& selectName = mySequence.myItems.at(mySequence.selectItemNum).name;
				int& selectStartFrame = mySequence.myItems.at(mySequence.selectItemNum).mFrameStart;
				int& selectEndFrame = mySequence.myItems.at(mySequence.selectItemNum).mFrameEnd;

				// �폜���������Ă��Ȃ���Ε\��
				if (!isSkip)
				{
					ImGui::InputText(u8"���O", (char*)selectName.c_str(), sizeof(selectName));

					ImGui::DragInt(u8"�J�n�t���[��", &selectStartFrame);
					ImGui::DragInt(u8"�I���t���[��", &selectEndFrame);


					// --- Sphere ---
					if (mySequence.myItems.at(mySequence.selectItemNum).mType == static_cast<int>(SequencerItemType::Sphere))
					{
						int index = mySequence.myItems.at(mySequence.selectItemNum).mTypeIndex;

						// �l�̃Z�b�g
						model->animationClips.at(animationClipIndex).spheres.at(index).name = selectName.c_str();
						model->animationClips.at(animationClipIndex).spheres.at(index).startFrame = selectStartFrame;
						model->animationClips.at(animationClipIndex).spheres.at(index).endFrame = selectEndFrame;

						float& radius = model->animationClips.at(animationClipIndex).spheres.at(index).radius;
						DirectX::XMFLOAT3& position = model->animationClips.at(animationClipIndex).spheres.at(index).position;
						DirectX::XMFLOAT4& color = model->animationClips.at(animationClipIndex).spheres.at(index).color;
						std::string bindBoneName = model->animationClips.at(animationClipIndex).spheres.at(index).bindBoneName;

						ImGui::DragFloat(u8"���a", &radius, 0.01f);
						ImGui::DragFloat3(u8"POSITION", &position.x, 0.01f);
						ImGui::ColorEdit4(u8"COLOR", &color.x);
						ImGui::InputText(u8"BONE���O", (char*)bindBoneName.c_str(), sizeof(bindBoneName));

						model->animationClips.at(animationClipIndex).spheres.at(index).bindBoneName = bindBoneName.c_str();
					}

					// --- Effect ---
					if (mySequence.myItems.at(mySequence.selectItemNum).mType == static_cast<int>(SequencerItemType::Effect))
					{
						int index = mySequence.myItems.at(mySequence.selectItemNum).mTypeIndex;

						// �l�̃Z�b�g
						model->animationClips.at(animationClipIndex).animEffects.at(index).name = selectName.c_str();
						model->animationClips.at(animationClipIndex).animEffects.at(index).startFrame = selectStartFrame;
						model->animationClips.at(animationClipIndex).animEffects.at(index).endFrame = selectEndFrame;

						EffectType& effecType = model->animationClips.at(animationClipIndex).animEffects.at(index).effectType;
						float& radius = model->animationClips.at(animationClipIndex).animEffects.at(index).scale;
						DirectX::XMFLOAT3& position = model->animationClips.at(animationClipIndex).animEffects.at(index).position;
						DirectX::XMFLOAT3& angle = model->animationClips.at(animationClipIndex).animEffects.at(index).angle;

						std::string effectTypeName[] = { "Hit1", "Move", "PowerUp", "Sleep", "Test1"};
						int effectTypeCount = static_cast<int>(EffectType::LAST);

						if (ImGui::BeginCombo(u8"�G�t�F�N�g�^�C�v", effectTypeName[static_cast<int>(effecType)].c_str()))
						{
							for (int i = 0; i < IM_ARRAYSIZE(effectTypeName); i++)
							{
								const bool isSelected = (model->fbxUnit == i);
								if (ImGui::Selectable(effectTypeName[i].c_str(), isSelected))
								{
									model->animationClips.at(animationClipIndex).animEffects.at(index).effectType = static_cast<EffectType>(i);
								}
								if (isSelected)
								{
									ImGui::SetItemDefaultFocus();
								}
							}
							ImGui::EndCombo();
						}

						ImGui::DragFloat(u8"�X�P�[��", &radius, 0.01f);
						ImGui::DragFloat3(u8"POSITION", &position.x, 0.01f);
						ImGui::DragFloat3(u8"ANGLE", &angle.x, 0.01f);
					}

					// --- SE ---
					if (mySequence.myItems.at(mySequence.selectItemNum).mType == static_cast<int>(SequencerItemType::SE))
					{
						int index = mySequence.myItems.at(mySequence.selectItemNum).mTypeIndex;

						// �l�̃Z�b�g
						model->animationClips.at(animationClipIndex).animSEs.at(index).name = selectName.c_str();
						model->animationClips.at(animationClipIndex).animSEs.at(index).startFrame = selectStartFrame;
						model->animationClips.at(animationClipIndex).animSEs.at(index).endFrame = selectEndFrame;

						MUSIC_LABEL& musicType = model->animationClips.at(animationClipIndex).animSEs.at(index).musicType;
						

						std::string musicTypeName[] = { "TEST_MUISC", "WEAPON" };

						if (ImGui::BeginCombo(u8"���ʉ��^�C�v", musicTypeName[static_cast<int>(musicType)].c_str()))
						{
							for (int i = 0; i < IM_ARRAYSIZE(musicTypeName); i++)
							{
								const bool isSelected = (model->fbxUnit == i);
								if (ImGui::Selectable(musicTypeName[i].c_str(), isSelected))
								{
									model->animationClips.at(animationClipIndex).animSEs.at(index).musicType = static_cast<MUSIC_LABEL>(i);
								}
								if (isSelected)
								{
									ImGui::SetItemDefaultFocus();
								}
							}
							ImGui::EndCombo();
						}
					}
				}
			}
		}
	}
	ImGui::End();

	//----------------------------------------------------------
	// �M�Y��
	//----------------------------------------------------------
	DebugGizmo();

	//----------------------------------------------------------
	// �^�C�����C��
	//----------------------------------------------------------
	DebugTimeLine();
}


// �^�C�����C������
void SceneAnimation::DebugTimeLine()
{
	//----------------------------------------------------------
	// �^�C�����C��
	//----------------------------------------------------------
	{
		// --- window�t���O�ݒ� ---
		static ImGuiWindowFlags windowFlags =
			//ImGuiWindowFlags_MenuBar |		// �����L���ɂ���ƃ��j���[�o�[��ǉ��ł���
			ImGuiDockNodeFlags_PassthruCentralNode |
			//ImGuiWindowFlags_NoDocking |
			//ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;


		// --- dockNode�t���O�ݒ� ---
		const ImGuiDockNodeFlags dockFlags = ImGuiDockNodeFlags_PassthruCentralNode;

		// --- ImGui�̕\���ݒ� ---
		ImGui::SetNextWindowBgAlpha(0.0f);					// �w�i�A���t�@�̐ݒ�

		// --- DockSpace�̎��̓X�^�C���̐ݒ�? ---
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		// --- DockSpace�̍쐬 ---
		ImGui::Begin(u8"�^�C�����C��", NULL, windowFlags);
		ImGui::PopStyleVar(3);	// ���̓X�^�C���̓K�p?

		ImGuiID dockSpaceID = ImGui::GetID("DockTimeline");
		ImGui::DockSpace(dockSpaceID, ImVec2(0.f, 0.f), dockFlags);

		ImGui::End();
	}

	//----------------------------------------------------------
	// �A�j���[�V�����ݒ�
	//----------------------------------------------------------
	ImGui::Begin(u8"�A�j���[�V�����ݒ�");
	{
		if (ImGui::Button(u8"�Đ�")) {
			isPlay = true;
			if (currentFrameInt >= mySequence.mFrameMax)
			{
				currentFrameInt = 0;
				currentFrameFloat = 0.0f;
			}
		}
		ImGui::SameLine();
		if (ImGui::Button(u8"��~")) {
			isPlay = false;
		}
		ImGui::SameLine();
		ImGui::Checkbox(u8"���[�v", &isLoop);
		
		
		ImGui::PushItemWidth(130);
		ImGui::DragFloat(u8"�^�C���X�P�[��", &timeScale, 0.01f, 0.0f, 3.0f);

		ImGui::InputInt(u8"�t���[��", &currentFrameInt);
		ImGui::InputInt(u8"�J�n�t���[��", &mySequence.mFrameMin);
		ImGui::InputInt(u8"�I���t���[��", &mySequence.mFrameMax);
		ImGui::PopItemWidth();
	}
	ImGui::End();

	//----------------------------------------------------------
	// �A�j���[�V�����^�C�����C��
	//----------------------------------------------------------
	ImGui::Begin(u8"�A�j���[�V�����^�C�����C��");
	{
		Sequencer(&mySequence, &currentFrameInt, &expanded, &selectedEntry,&firstFrame, ImSequencer::SEQUENCER_EDIT_STARTEND |ImSequencer::SEQUENCER_ADD | ImSequencer::SEQUENCER_DEL |ImSequencer::SEQUENCER_COPYPASTE | ImSequencer::SEQUENCER_CHANGE_FRAME);
		// add a UI to edit that particular item
		if (selectedEntry != -1)
		{
			const MySequence::MySequenceItem& item = mySequence.myItems[selectedEntry];
			ImGui::Text("I am a %s, please edit me", SequencerItemTypeNames[item.mType]);
			// switch (type) ....
		}
	}
	ImGui::End();


	if (mySequence.myItems.size() == 0)
	{
		mySequence.Add("", 0, -10, -10);
	}
}

// �M�Y������
void SceneAnimation::DebugGizmo()
{
	//---------------------------------------------
	// DockSpace ����
	//---------------------------------------------

	// --- window�t���O�ݒ� ---
	static ImGuiWindowFlags windowFlags =
		//ImGuiWindowFlags_MenuBar |		// �����L���ɂ���ƃ��j���[�o�[��ǉ��ł���
		ImGuiDockNodeFlags_PassthruCentralNode |
		//ImGuiWindowFlags_NoDocking |
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;


	// --- dockNode�t���O�ݒ� ---
	const ImGuiDockNodeFlags dockFlags = ImGuiDockNodeFlags_PassthruCentralNode;

	// --- ImGui�̕\���ݒ� ---
	ImGuiViewport* viewport = ImGui::GetMainViewport();	// ���C���r���[�̒l���擾
	ImGui::SetNextWindowPos(viewport->WorkPos);			// �|�W�V�����̐ݒ�
	ImGui::SetNextWindowSize(viewport->Size, ImGuiCond_Always);		// �T�C�Y�̐ݒ�
	ImGui::SetNextWindowViewport(viewport->ID);			// ID�̐ݒ�
	ImGui::SetNextWindowBgAlpha(0.0f);					// �w�i�A���t�@�̐ݒ�

	// --- DockSpace�̎��̓X�^�C���̐ݒ�? ---
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));


	//---------------------------------------------
	// �M�Y������
	//---------------------------------------------

	// ---- �M�Y���ϐ� ----
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
	static bool useSnap = false;
	static float snap[3] = { 1.f, 1.f, 1.f };
	static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
	static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
	static bool boundSizing = false;
	static bool boundSizingSnap = false;

	ImGuiIO& io = ImGui::GetIO();
	float viewManipulateRight = io.DisplaySize.x;
	float viewManipulateTop = 0;
	static ImGuiWindowFlags gizmoWindowFlags = 0;

	// view projection �s����g����`�ɕϊ�
	DirectX::XMFLOAT4X4 viewMatrix = Camera::Instance().GetView();
	DirectX::XMFLOAT4X4 viewProjection = Camera::Instance().GetProjection();

	float* view = reinterpret_cast<float*>(&viewMatrix);
	float* projection = reinterpret_cast<float*>(&viewProjection);

	// �A�C�e���p�s��
	DirectX::XMFLOAT4X4		itemTransform4x4 =
	{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
	// �A�C�e���`��p���W
	DirectX::XMFLOAT3 position;


	//---------------------------------------------
	// DockSpace�����J�n
	//---------------------------------------------

	// --- DockSpace�̍쐬 ---
	ImGui::Begin(u8"�O���b�h�X�y�[�X", NULL, windowFlags);
	ImGui::PopStyleVar(3);	// ���̓X�^�C���̓K�p?

	ImGuiID dockSpaceID = ImGui::GetID("DockGrid");
	ImGui::DockSpace(dockSpaceID, ImVec2(0.f, 0.f), dockFlags);


	//---------------------------------------------
	// �M�Y�������J�n
	//---------------------------------------------
	if (model)
	{
		if (mySequence.selectItemNum != -1)
		{
			if (mySequence.myItems.at(mySequence.selectItemNum).mType == static_cast<int>(SequencerItemType::Sphere))
			{
				int index = mySequence.myItems.at(mySequence.selectItemNum).mTypeIndex;
				int startFrame = model->animationClips.at(animationClipIndex).spheres.at(index).startFrame;
				int endFrame = model->animationClips.at(animationClipIndex).spheres.at(index).endFrame;

				// �A�C�e���̃t���[���͈͊O�Ȃ珈�����Ȃ�
				if (!(currentFrameInt < startFrame || currentFrameInt > endFrame))
				{
					int index = mySequence.myItems.at(mySequence.selectItemNum).mTypeIndex;
					position = model->animationClips.at(animationClipIndex).spheres.at(index).position;

					// �ʒu�s����쐬(��]�A�X�P�[���ύX�͂��Ȃ����߂��ꂪ���̂܂܃��[���h�s��ɂȂ�)
					DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
					DirectX::XMStoreFloat4x4(&itemTransform4x4, T);
					// ���[���h�s����g����`�ɕϊ�
					float* itemTransform = reinterpret_cast<float*>(&itemTransform4x4);

					// --- �M�Y���ϐ��ݒ� ---
					ImGuizmo::SetDrawlist();
					float windowWidth = (float)Framework::Instance().windowWidth;
					float windowHeight = (float)Framework::Instance().windowHeight;
					ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);
					viewManipulateRight = ImGui::GetWindowPos().x + windowWidth;
					viewManipulateTop = ImGui::GetWindowPos().y;
					ImGuiWindow* window = ImGui::GetCurrentWindow();
					gizmoWindowFlags = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(window->InnerRect.Min, window->InnerRect.Max) ? ImGuiWindowFlags_NoMove : 0;

					// --- �M�Y���`�� ---
					ImGuizmo::Manipulate(view, projection, ImGuizmo::TRANSLATE, mCurrentGizmoMode, itemTransform, NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);

					// --- �M�Y���̒l�𔽉f ---
					model->animationClips.at(animationClipIndex).spheres.at(index).position.x = itemTransform[12];
					model->animationClips.at(animationClipIndex).spheres.at(index).position.y = itemTransform[13];
					model->animationClips.at(animationClipIndex).spheres.at(index).position.z = itemTransform[14];
				}
			}
		}

		if (mySequence.myItems.at(mySequence.selectItemNum).mType == static_cast<int>(SequencerItemType::Effect))
		{
			int index = mySequence.myItems.at(mySequence.selectItemNum).mTypeIndex;
			int startFrame = model->animationClips.at(animationClipIndex).animEffects.at(index).startFrame;
			int endFrame = model->animationClips.at(animationClipIndex).animEffects.at(index).endFrame;

			// �A�C�e���̃t���[���͈͊O�Ȃ珈�����Ȃ�
			if (!(currentFrameInt < startFrame || currentFrameInt > endFrame))
			{
				int index = mySequence.myItems.at(mySequence.selectItemNum).mTypeIndex;
				position = model->animationClips.at(animationClipIndex).animEffects.at(index).position;

				// �ʒu�s����쐬(��]�A�X�P�[���ύX�͂��Ȃ����߂��ꂪ���̂܂܃��[���h�s��ɂȂ�)
				DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
				DirectX::XMStoreFloat4x4(&itemTransform4x4, T);
				// ���[���h�s����g����`�ɕϊ�
				float* itemTransform = reinterpret_cast<float*>(&itemTransform4x4);

				// --- �M�Y���ϐ��ݒ� ---
				ImGuizmo::SetDrawlist();
				float windowWidth = (float)Framework::Instance().windowWidth;
				float windowHeight = (float)Framework::Instance().windowHeight;
				ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);
				viewManipulateRight = ImGui::GetWindowPos().x + windowWidth;
				viewManipulateTop = ImGui::GetWindowPos().y;
				ImGuiWindow* window = ImGui::GetCurrentWindow();
				gizmoWindowFlags = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(window->InnerRect.Min, window->InnerRect.Max) ? ImGuiWindowFlags_NoMove : 0;

				// --- �M�Y���`�� ---
				ImGuizmo::Manipulate(view, projection, ImGuizmo::TRANSLATE, mCurrentGizmoMode, itemTransform, NULL, useSnap ? &snap[0] : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);

				// --- �M�Y���̒l�𔽉f ---
				model->animationClips.at(animationClipIndex).animEffects.at(index).position.x = itemTransform[12];
				model->animationClips.at(animationClipIndex).animEffects.at(index).position.y = itemTransform[13];
				model->animationClips.at(animationClipIndex).animEffects.at(index).position.z = itemTransform[14];
			}
		}
	}
	ImGui::End();
}

// ���f���N���A
void SceneAnimation::ModelClear()
{
	if (model)
	{
		delete model;
		model = nullptr;
	}

	transform =
	{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
	modelScale = 1.0f;
	modelAngle = { 0.0f, 0.0f, 0.0f };
	modelPosition = { 0.0f, 0.0f, 0.0f };

	animationClipIndex = 0;
	currentFrameInt = 0;
	currentFrameFloat = 0.0f;

	isLoop = false;
	isPlay = false;

	timeScale = 1.0f;
}

// �G�t�F�N�g�t���O�N���A
void SceneAnimation::ClearEffectFlag()
{
	int effectCount = model->animationClips.at(animationClipIndex).animEffects.size();

	for (int i = 0; i < effectCount; i++)
	{
		model->animationClips.at(animationClipIndex).animEffects.at(i).isPlay = false;
	}
}

// ���ʉ��t���O�N���A
void SceneAnimation::ClearSEFlag()
{
	int seCount = model->animationClips.at(animationClipIndex).animSEs.size();

	for (int i = 0; i < seCount; i++)
	{
		model->animationClips.at(animationClipIndex).animSEs.at(i).isPlay = false;
	}
}


