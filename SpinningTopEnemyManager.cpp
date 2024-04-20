#include "SpinningTopEnemyManager.h"
#include "Library/ImGui/Include/imgui.h"
#include "Collision.h"
#include "EnemySlime.h"
#include "Library/Graphics/Graphics.h"
#include "ObstacleManager.h"

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
void SpinningTopEnemyManager::Render(bool drawShadow)
{
	for (SpinningTopEnemy* enemy : enemies)
	{
		enemy->Render(drawShadow);
		if (drawDebugPrimitive)
		{
			enemy->DrawDebugPrimitive();
		}
	}

	// �G���m�̏Փˏ���
	CollisionEnemyVsEnemeis();

	CollisionEnemyVsObstacle();
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

	ObstacleManager& obsM = ObstacleManager::Instance();

	hit.distance = FLT_MAX;
	int obsCount = obsM.GetObstacleCount();
	for (SpinningTopEnemy* enemy : enemies)
	{
		for (int i = 0; i < obsCount; i++)
		{
			Obstacle* obs = obsM.GetObstacle(i);

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
			DirectX::XMFLOAT3 velocityA;
			DirectX::XMFLOAT3 velocityB;
			if (Collision::RepulsionSphereVsSphere(
				enemyA->GetPosition(),
				enemyA->GetRadius(),
				1,
				enemyB->GetPosition(),
				enemyB->GetRadius(),
				1,
				velocityA,
				velocityB
			))
			{
				// �����o����̈ʒu�ݒ�
				DirectX::XMFLOAT3 enemyAVel = enemyA->GetVelocity();
				DirectX::XMVECTOR V_A = DirectX::XMLoadFloat3(&enemyAVel);
				DirectX::XMFLOAT3 enemyBVel = enemyB->GetVelocity();
				DirectX::XMVECTOR V_B = DirectX::XMLoadFloat3(&enemyBVel);

				DirectX::XMVECTOR AddA = DirectX::XMLoadFloat3(&velocityA);
				DirectX::XMVECTOR AddB = DirectX::XMLoadFloat3(&velocityB);

				V_A = DirectX::XMVectorAdd(V_A, AddA);
				DirectX::XMStoreFloat3(&velocityA, V_A);
				V_B = DirectX::XMVectorAdd(V_B, AddB);
				DirectX::XMStoreFloat3(&velocityB, V_B);

				enemyA->SetVelocity(velocityA);
				enemyB->SetVelocity(velocityB);
			}
		}
	}

}

void SpinningTopEnemyManager::CollisionEnemyVsObstacle()
{
	ObstacleManager& obsM = ObstacleManager::Instance();

	for (SpinningTopEnemy* enemyA : enemies)
	{
		int obsCount = obsM.GetObstacleCount();
		for(int i = 0; i < obsCount; i++)
		{
			Obstacle* obs = obsM.GetObstacle(i);

			if (!obs->isCollision) continue;

			// �Փˏ���
			DirectX::XMFLOAT3 velocityA;
			if (Collision::StaticRepulsionSphereVsSphere(
				enemyA->GetPosition(),
				enemyA->GetRadius(),
				obs->GetPosition(),
				obs->GetRadius(),
				velocityA
			))
			{
				// �����o����̈ʒu�ݒ�
				DirectX::XMFLOAT3 enemyAVel = enemyA->GetVelocity();
				DirectX::XMVECTOR V_A = DirectX::XMLoadFloat3(&enemyAVel);

				DirectX::XMVECTOR AddA = DirectX::XMLoadFloat3(&velocityA);

				V_A = DirectX::XMVectorAdd(V_A, AddA);
				DirectX::XMStoreFloat3(&velocityA, V_A);

				enemyA->SetVelocity(velocityA);
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
		ImGui::Checkbox(("sync"), &Graphics::Instance().sync);
		ImGui::Checkbox(("drawDebugPrimitive"), &drawDebugPrimitive);

		uint32_t enemyCount = enemies.size();
		for (int i = 0; i < enemyCount; i++)
		{
			SpinningTopEnemy* enemy = enemies.at(i);
			if (ImGui::CollapsingHeader(("SpinningTopEnemy - " + std::to_string(i)).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				// �ʒu
				DirectX::XMFLOAT3 position = enemy->GetPosition();
				DirectX::XMFLOAT3 vel = enemy->GetVelocity();
				ImGui::DragFloat3("Position", &position.x, 0.1f);
				ImGui::InputFloat3("velocity", &vel.x);
				ImGui::DragFloat3("Target", &enemy->targetPosition.x, 0.1f);
				ImGui::DragFloat("rotationSpeed", &enemy->rotationSpeed, 0.1f);

				if (ImGui::Button("damage"))
				{
					enemy->ApplyDamage(1, 0.0f);
				}
			}
		}
	}

	ImGui::End();


}