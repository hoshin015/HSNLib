#include "ObstacleManager.h"
#include "Library/ImGui/Include/imgui.h"
#include "Collision.h"

// �X�V����
void ObstacleManager::Update()
{
	// �X�V����
	for (Obstacle* obstacle : obstacles)
	{
		obstacle->Update();
	}

	// �j������
	for (Obstacle* obstacle : removes)
	{
		std::vector<Obstacle*>::iterator it = std::find(obstacles.begin(), obstacles.end(), obstacle);

		if (it != obstacles.end())
		{
			obstacles.erase(it);
		}
		// �e�ۂ̔j������
		delete obstacle;
	}
	// �j�����X�g���N���A
	removes.clear();

	// ��Q�����m�̓����蔻��
	CollisionObstacleVSObstacle();
}

//�@�`�揈��
void ObstacleManager::Render()
{
	for (Obstacle* obstacle : obstacles)
	{
		obstacle->Render();
		obstacle->DrawDebugPrimitive();
	}
}



// �G�l�~�[�o�^
void ObstacleManager::Register(Obstacle* obstacle)
{
	obstacles.emplace_back(obstacle);
}


// �G�l�~�[�폜
void ObstacleManager::Remove(Obstacle* obstacle)
{
	// �j�����X�g�ɒǉ�
	removes.insert(obstacle);
}

// �G�l�~�[�S�폜
void ObstacleManager::Clear()
{
	for (Obstacle* obstacle : obstacles)
	{
		delete obstacle;
	}
	obstacles.clear();
}

void ObstacleManager::CollisionObstacleVSObstacle()
{
	for (Obstacle* obsA : obstacles)
	{
		for (Obstacle* obsB : obstacles)
		{
			if (obsA == obsB) continue;

			// �Փˏ���
			DirectX::XMFLOAT3 outPositionA;
			DirectX::XMFLOAT3 outPositionB;
			if (Collision::RepulsionSphereVsSphere(
				obsA->GetPosition(),
				obsA->GetRadius(),
				1.0f,
				obsB->GetPosition(),
				obsB->GetRadius(),
				1.0f,
				outPositionA,
				outPositionB)
				)
			{
				// �����蔻��̂Ȃ���Q���̂� velocity�ɉe����^����(����ł̓p�[�c�݂̂�����A�������₵����ύX���K�v)
				if (!obsA->isCollision) obsA->velocity = outPositionA;
				if (!obsB->isCollision) obsB->velocity = outPositionB;
			}
		}
	}
}

// �f�o�b�O�pGUI�`��
void ObstacleManager::DrawDebugGui()
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("ObstacleManager", nullptr, ImGuiWindowFlags_None))
	{
		uint32_t obstacleCount = obstacles.size();
		for (int i = 0; i < obstacleCount; i++)
		{
			Obstacle* obstacle = obstacles.at(i);
			if (ImGui::CollapsingHeader(("Obstacle - " + std::to_string(i)).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				// �ʒu
				DirectX::XMFLOAT3 position = obstacle->GetPosition();
				ImGui::DragFloat3("Position", &position.x, 0.1f);
			}
		}
	}
	ImGui::End();
}