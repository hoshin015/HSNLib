#include "SpinningTopEnemyManager.h"
#include "Library/ImGui/Include/imgui.h"
#include "Collision.h"
#include "EnemySlime.h"

// �X�V����
void SpinningTopEnemyManager::Update()
{
	// �X�V����
	for (SpinningTopEnemy* enemy : enemies)
	{
		enemy->Update();
	}

	// �j������
	for (SpinningTopEnemy* enemy : removes)
	{
		std::vector<SpinningTopEnemy*>::iterator it = std::find(enemies.begin(), enemies.end(), enemy);

		if (it != enemies.end())
		{
			enemies.erase(it);
		}
		// �e�ۂ̔j������
		delete enemy;
	}
	// �j�����X�g���N���A
	removes.clear();
}

//�@�`�揈��
void SpinningTopEnemyManager::Render()
{
	for (SpinningTopEnemy* enemy : enemies)
	{
		enemy->Render();
	}

	// �G���m�̏Փˏ���
	CollisionEnemyVsEnemeis();
}

// �G�l�~�[�o�^
void SpinningTopEnemyManager::Register(SpinningTopEnemy* enemy)
{
	enemies.emplace_back(enemy);
	enemy->id = enemies.size() - 1;
}


// �G�l�~�[�폜
void SpinningTopEnemyManager::Remove(SpinningTopEnemy* enemy)
{
	// �j�����X�g�ɒǉ�
	removes.insert(enemy);
}

// �G�l�~�[�S�폜
void SpinningTopEnemyManager::Clear()
{
	for (SpinningTopEnemy* enemy : enemies)
	{
		delete enemy;
	}
	enemies.clear();
}

void SpinningTopEnemyManager::SortPlaySortEnemiesByDistanceToPlayerEnemy(DirectX::XMFLOAT3 playerPosition)
{
	// �S�Ă�enemy��player�Ƃ̋������v��
	DirectX::XMVECTOR playerPos = DirectX::XMLoadFloat3(&playerPosition);
	for (SpinningTopEnemy* enemy : enemies)
	{
		DirectX::XMVECTOR enemyPos = DirectX::XMLoadFloat3(&enemy->GetPosition());

		DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(enemyPos, playerPos);
		DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);
		DirectX::XMStoreFloat(&enemy->playerDistance, LengthSq);
	}

	int enemyCount = enemies.size();
	for (int i = 0; i < enemyCount; i++)
	{
		SpinningTopEnemy* enemyA = enemies.at(i);

		// �S�Ă�enemy�Ɣ�r
		for (int j = 0; j < enemyCount; j++)
		{
			SpinningTopEnemy* enemyB = enemies.at(j);

			if (enemyA == enemyB) continue;

			if (enemyA->playerDistance < enemyB->playerDistance)
			{
				std::swap(enemies[i], enemies[j]);
			}
		}
	}
}

bool SpinningTopEnemyManager::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	bool result = false;

	hit.distance = FLT_MAX;
	for (SpinningTopEnemy* enemy : enemies)
	{
		HitResult hitResult;
		if (enemy->RayCast(start, end, hitResult))
		{
			if (hit.distance > hitResult.distance)
			{
				hit = hitResult;
				result = true;
			}
		}
	}

	return result;
}

bool SpinningTopEnemyManager::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit, int id)
{
	bool result = false;

	hit.distance = FLT_MAX;
	for (SpinningTopEnemy* enemy : enemies)
	{
		if (enemy->id == id) continue;

		HitResult hitResult;
		if (enemy->RayCast(start, end, hitResult))
		{
			if (hit.distance > hitResult.distance)
			{
				hit = hitResult;
				result = true;
			}
		}
	}

	return result;
}

// �G�l�~�[���m�̏Փ˔���
void SpinningTopEnemyManager::CollisionEnemyVsEnemeis()
{
	for (SpinningTopEnemy* enemyA : enemies)
	{
		for (SpinningTopEnemy* enemyB : enemies)
		{
			if (enemyA == enemyB) continue;

			// �Փˏ���
			DirectX::XMFLOAT3 outPosition;
			if (Collision::IntersectCylinderVsCylinder(
				enemyA->GetPosition(),
				enemyA->GetRadius(),
				enemyA->GetHeight(),
				enemyB->GetPosition(),
				enemyB->GetRadius(),
				enemyB->GetHeight(),
				outPosition)
				)
			{
				// �����o����̈ʒu�ݒ�
				enemyB->SetPosition(outPosition);
			}
		}
	}

}


// �f�o�b�O�pGUI�`��
void SpinningTopEnemyManager::DrawDebugGui()
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("SpinningTopEnemyManager", nullptr, ImGuiWindowFlags_None))
	{
		uint32_t enemyCount = enemies.size();
		for (int i = 0; i < enemyCount; i++)
		{
			SpinningTopEnemy* enemy = enemies.at(i);
			if (ImGui::CollapsingHeader(("SpinningTopEnemy - " + std::to_string(i)).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				// �ʒu
				DirectX::XMFLOAT3 position = enemy->GetPosition();
				ImGui::DragFloat3("Position", &position.x, 0.1f);
				ImGui::InputFloat("Distance", &enemy->playerDistance);
				ImGui::DragFloat3("Target", &enemy->targetPosition.x, 0.1f);
				ImGui::DragFloat("rotationSpeed", &enemy->rotationSpeed, 0.1f);
			}
		}
	}
	ImGui::End();
}