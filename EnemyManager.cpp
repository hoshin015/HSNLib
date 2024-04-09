#include "EnemyManager.h"
#include "Library/ImGui/Include/imgui.h"
#include "Collision.h"
#include "EnemySlime.h"

// �X�V����
void EnemyManager::Update()
{
	// �X�V����
	for (Enemy* enemy : enemies)
	{
		enemy->Update();
	}

	// �j������
	for (Enemy* enemy : removes)
	{
		std::vector<Enemy*>::iterator it = std::find(enemies.begin(), enemies.end(), enemy);

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
void EnemyManager::Render()
{
	for (Enemy* enemy : enemies)
	{
		enemy->Render();
	}

	// �G���m�̏Փˏ���
	CollisionEnemyVsEnemeis();
}

// �G�l�~�[�o�^
void EnemyManager::Register(Enemy* enemy)
{
	enemies.emplace_back(enemy);
	enemy->id = enemies.size() - 1;
}


// �G�l�~�[�폜
void EnemyManager::Remove(Enemy* enemy)
{
	// �j�����X�g�ɒǉ�
	removes.insert(enemy);
}

// �G�l�~�[�S�폜
void EnemyManager::Clear()
{
	for (Enemy* enemy : enemies)
	{
		delete enemy;
	}
	enemies.clear();
}

void EnemyManager::SortPlaySortEnemiesByDistanceToPlayerEnemy(DirectX::XMFLOAT3 playerPosition)
{
	// �S�Ă�enemy��player�Ƃ̋������v��
	DirectX::XMVECTOR playerPos = DirectX::XMLoadFloat3(&playerPosition);
	for (Enemy* enemy : enemies)
	{
		DirectX::XMVECTOR enemyPos = DirectX::XMLoadFloat3(&enemy->GetPosition());

		DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(enemyPos, playerPos);
		DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);
		DirectX::XMStoreFloat(&enemy->playerDistance, LengthSq);
	}

	int enemyCount = enemies.size();
	for (int i = 0; i < enemyCount; i++)
	{
		Enemy* enemyA = enemies.at(i);

		// �S�Ă�enemy�Ɣ�r
		for (int j = 0; j < enemyCount; j++)
		{
			Enemy* enemyB = enemies.at(j);

			if (enemyA == enemyB) continue;

			if (enemyA->playerDistance < enemyB->playerDistance)
			{
				std::swap(enemies[i], enemies[j]);
			}
		}
	}
}

bool EnemyManager::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	bool result = false;

	hit.distance = FLT_MAX;
	for (Enemy* enemy : enemies)
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

bool EnemyManager::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit, int id)
{
	bool result = false;

	hit.distance = FLT_MAX;
	for (Enemy* enemy : enemies)
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
void EnemyManager::CollisionEnemyVsEnemeis()
{
	for (Enemy* enemyA : enemies)
	{
		for (Enemy* enemyB : enemies)
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
void EnemyManager::DrawDebugGui()
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("EnemyManager", nullptr, ImGuiWindowFlags_None))
	{
		if (ImGui::Button("Add", ImVec2(100, 20)))
		{
			EnemySlime* slime = new EnemySlime();
			DirectX::XMFLOAT3 addPosition = { static_cast<float>(rand() % 3), 0, static_cast<float>(rand() % 3) };
			slime->SetPosition(addPosition);
			Register(slime);
		}
		ImGui::SameLine();
		if (ImGui::Button("Clear", ImVec2(100, 20)))
		{
			Clear();
		}

		uint32_t enemyCount = enemies.size();
		for (int i = 0; i < enemyCount; i++)
		{
			Enemy* enemy = enemies.at(i);
			if (ImGui::CollapsingHeader(("Enemy - " + std::to_string(i)).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				// �ʒu
				DirectX::XMFLOAT3 position = enemy->GetPosition();
				ImGui::DragFloat3("Position", &position.x, 0.1f);
				ImGui::InputFloat("Distance", &enemy->playerDistance);
			}
		}
	}
	ImGui::End();
}