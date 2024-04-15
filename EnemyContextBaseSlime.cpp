#include "EnemyContextBaseSlime.h"
#include "Library/Graphics/Graphics.h"
#include "Library/MemoryLeak.h"
#include "Library/Timer.h"
#include "Library/3D/DebugPrimitive.h"
#include "Library/3D/ResourceManager.h"
#include "Library/3D/LineRenderer.h"
#include "Library/ImGui/Include/imgui.h"
#include "PlayerManager.h"
#include "StageManager.h"
#include "EnemyManager.h"

EnemyContextBaseSlime::EnemyContextBaseSlime()
{
	health = 50;
	maxHealth = 50;

	maxMoveSpeed = 100;
	acceleration = 100;

	//SetAngle({ 0, 180, 0 });

	// �p�x�v�Z
	CalcOneRadius();

	model = ResourceManager::Instance().LoadModelResource("Data/Fbx/Slime/Slime.fbx");

	// �ҋ@�X�e�[�g�֑J��
	TransitionIdleState();
}

EnemyContextBaseSlime::~EnemyContextBaseSlime()
{
}

void EnemyContextBaseSlime::Update()
{
	// �X�e�[�g���Ƃ̏���
	switch (state)
	{
	case EnemyContextBaseSlime::State::Idle:
		UpdateIdleState();
		break;
	case EnemyContextBaseSlime::State::Move:
		UpdateMoveState();
		break;
	}

	// ���͍X�V����
	UpdateVelocity();
	// ���G���ԍX�V
	UpdateInvincibleTimer();

	UpdateAnimation();

	// �I�u�W�F�N�g�s��X�V
	UpdateTransform();
}

void EnemyContextBaseSlime::Render()
{
	using namespace DirectX;

	// --- Graphics �擾 ---
	Graphics& gfx = Graphics::Instance();

	// rasterizer �̐ݒ�
	gfx.SetRasterizer(RASTERIZER_STATE::CLOCK_TRUE_SOLID);

	model->Render(transform, { 1,1,1,1 }, &keyFrame);

	DebugPrimitive::Instance().AddCylinder(position, radius, height, { 1,0,0,1 });

	// ContextBase �`��
	DrawContext();

	// �f�o�b�O�pGUI�`��
	DrawDebugGui();
}

// �f�o�b�O�pGUI�`��
void EnemyContextBaseSlime::DrawDebugGui()
{
	ImGui::Begin("ContextBaseSlime");
	{
		if (ImGui::Button("tuiseki")) TransitionMoveState();
		ImGui::SameLine();
		if (ImGui::Button("Stop")) TransitionIdleState();

		// �E�B���h�E���ŕ`����J�n
		ImGui::BeginChild("Direction", ImVec2(300, 300), false);

		ImVec2 p = ImGui::GetCursorScreenPos();
		ImVec2 size = { ImGui::GetWindowSize().x - 1, ImGui::GetWindowSize().y - 1 };
		ImGui::GetWindowDrawList()->AddLine(p, ImVec2(p.x + size.x, p.y), IM_COL32(255, 0, 0, 255));
		ImGui::GetWindowDrawList()->AddLine(p, ImVec2(p.x, p.y + size.y), IM_COL32(255, 0, 0, 255));
		ImGui::GetWindowDrawList()->AddLine(ImVec2(p.x + size.x, p.y), ImVec2(p.x + size.x, p.y + size.y), IM_COL32(255, 0, 0, 255));
		ImGui::GetWindowDrawList()->AddLine(ImVec2(p.x, p.y + size.y), ImVec2(p.x + size.x, p.y + size.y), IM_COL32(255, 0, 0, 255));


		// ���S����̐���`��
		ImVec2 center = ImVec2(p.x + 150, p.y + 150);
		float ImRadius = 100.0f;

		// �~�̕`��
		ImGui::GetWindowDrawList()->AddCircle(center, ImRadius, IM_COL32(255, 255, 255, 255), 0, 2.0f);

		// �}�E�X�̈ʒu���擾���ē_��`��
		ImVec2 mousePos = ImGui::GetMousePos();
		ImGui::GetWindowDrawList()->AddCircleFilled(mousePos, 5.0f, IM_COL32(255, 0, 0, 255));

		float maxLength = 0.0f;
		for (int index = 0; index < numRays; index++)
		{
			if (maxLength < interest[index]) maxLength = interest[index];
		}

		// interest - danger �`��
		for (int index = 0; index < numRays; index++)
		{
			float radian = DirectX::XMConvertToRadians(rayDirections[index]);
			
			float dot = interest[index] - danger[index];
			dot /= maxLength;
			if (dot <= 0) continue;

			// ImGui �`��Ɏg�p���邽�� sinf �� - ����Z���Ă��Ȃ�
			ImVec2 end = ImVec2(center.x + cosf(radian) * ImRadius * dot, center.y + sinf(radian) * ImRadius * dot);

			ImGui::GetWindowDrawList()->AddLine(center, end, IM_COL32(0, 255, 0, 255));
		}

		// �ŏI�I�Ȉړ�������`��
		ImVec2 end = ImVec2(center.x + contextVelocity.x * ImRadius, center.y - contextVelocity.z * ImRadius);
		ImGui::GetWindowDrawList()->AddLine(center, end, IM_COL32(255, 255, 0, 255));

		// �E�B���h�E���ł̕`����I��
		ImGui::EndChild();


	}
	ImGui::End();
}

// ContextBase �`��
void EnemyContextBaseSlime::DrawContext()
{
	const DirectX::XMFLOAT4 white = DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1);
	const DirectX::XMFLOAT4 green = DirectX::XMFLOAT4(0, 1, 0, 1);
	const DirectX::XMFLOAT4 yellow = DirectX::XMFLOAT4(1, 1, 0, 1);

	LineRenderer& lineRenderer = LineRenderer::Instance();

	DirectX::XMVECTOR V, P;
	DirectX::XMFLOAT3 basePosition;
	DirectX::XMFLOAT3 position;

	// �x�[�X�̔��F���C���`��
	basePosition = GetPosition();
	basePosition.y += GetHeight() * 0.3f;
	for (int i = 0; i < numRays; i++)
	{
		float radian = DirectX::XMConvertToRadians(rayDirections[i]);

		position.x = basePosition.x + cosf(radian) * lookAhead;
		position.y = basePosition.y;
		position.z = basePosition.z + -sinf(radian) * lookAhead;

		lineRenderer.AddVertex(basePosition, white);

		lineRenderer.AddVertex(position, white);
	}

	float maxLength = 0.0f;
	for (int index = 0; index < numRays; index++)
	{
		if (maxLength < interest[index]) maxLength = interest[index];
	}

	// interest - danger �`��
	basePosition = GetPosition();
	basePosition.y += GetHeight() * 0.31f;
	for (int i = 0; i < numRays; i++)
	{
		float radian = DirectX::XMConvertToRadians(rayDirections[i]);

		float dot = interest[i] - danger[i];
		dot /= maxLength;
		if (dot <= 0) continue;

		position.x = basePosition.x + cosf(radian) * lookAhead * dot;
		position.y = basePosition.y;
		position.z = basePosition.z + -sinf(radian) * lookAhead * dot;

		lineRenderer.AddVertex(basePosition, green);

		lineRenderer.AddVertex(position, green);
	}

	// �ŏI�I�Ȉړ����� �`��
	basePosition = GetPosition();
	basePosition.y += GetHeight() * 0.32f;
	position.x = basePosition.x + contextVelocity.x * lookAhead;
	position.y = basePosition.y;
	position.z = basePosition.z + contextVelocity.z * lookAhead;

	lineRenderer.AddVertex(basePosition, yellow);
	lineRenderer.AddVertex(position, yellow);
}



// ���S����
void EnemyContextBaseSlime::OnDead()
{
	Destroy();
}


//==========================================================================
//
//�@�X�e�[�g����
// 
//==========================================================================

// �ҋ@�X�e�[�g�֑J��
void EnemyContextBaseSlime::TransitionIdleState()
{
	// �X�e�[�g�ύX
	state = State::Idle;

	// �u�����h�ݒ�
	blendAnimationIndex = currentAnimationIndex;
	// �A�j���[�V�����ύX
	PlayAnimation(static_cast<int>(EnemyContextBaseSlimeAnim::IdleNormal), true);
}

// �ҋ@�X�e�[�g�X�V����
void EnemyContextBaseSlime::UpdateIdleState()
{
	// �^�[�Q�b�g�`�F�b�N
	RayTarget();

	// �����`�F�b�N
	InterestCheck();

	// �댯�x�`�F�b�N
	AccelerationCheck();

	// �ŏI�I�Ȉړ��x�N�g���Z�o
	CalcContextVec();
}

// �ړ��X�e�[�g�֑J��
void EnemyContextBaseSlime::TransitionMoveState()
{
	// �X�e�[�g�ύX
	state = State::Move;

	// �u�����h�ݒ�
	blendAnimationIndex = currentAnimationIndex;
	// �A�j���[�V�����ύX
	PlayAnimation(static_cast<int>(EnemyContextBaseSlimeAnim::WalkFWD), true);
}

// �ړ��X�e�[�g�X�V����
void EnemyContextBaseSlime::UpdateMoveState()
{
	// �^�[�Q�b�g�`�F�b�N
	RayTarget();

	// �����`�F�b�N
	InterestCheck();

	// �댯�x�`�F�b�N
	AccelerationCheck();

	// �ŏI�I�Ȉړ��x�N�g���Z�o
	CalcContextVec();

	velocity = contextVelocity;

	// ���񏈗�
	Turn(velocity.x, velocity.z, 720);

	//EndChaseCheck();
}

//==========================================================================
//
//�@ContextBase
// 
//==========================================================================

// �v���C���[�ւ̃��C�L���X�g
void EnemyContextBaseSlime::RayTarget()
{
	Player* player = PlayerManager::Instance().GetPlayer(0);
	DirectX::XMFLOAT3 playerPos = player->GetPosition();
	DirectX::XMFLOAT3 enemyPos = GetPosition();

	// ���C�̊J�n�ʒu�͑�����菭����
	DirectX::XMFLOAT3 start = { position.x, position.y + stepOffset, position.z };

	// ���C�̏I�_�ʒu�̓^�[�Q�b�g�̈ʒu
	DirectX::XMFLOAT3 end = { playerPos.x, position.y + stepOffset, playerPos.z };

	// ���C�L���X�g�ɂ��n�ʔ���
	HitResult hit;
	if (!StageManager::Instance().RayCast(start, end, hit))
	{
		// ���C�L���X�g�ɃX�e�[�W���������ĂȂ���΁A�ʒu���X�V
		lastTargetPosition = playerPos;
	}
	lastTargetPosition = playerPos;
}

// �p�x�v�Z
void EnemyContextBaseSlime::CalcOneRadius()
{
	int index = 0;
	float oneRadius = 360.0f / numRays;
	for (float radius = 0.0f; radius < 360.0f; radius += oneRadius)
	{
		rayDirections[index] = radius;
		index++;
	}
}

// �����`�F�b�N
void EnemyContextBaseSlime::InterestCheck()
{
	DirectX::XMFLOAT3 targetPos = lastTargetPosition;
	DirectX::XMFLOAT3 enemyPos = GetPosition();

	

	// interest �����߂�
	for (int index = 0; index < numRays; index++)
	{
		float radian = DirectX::XMConvertToRadians(rayDirections[index]);

		DirectX::XMVECTOR BaseV = { cosf(radian) * lookAhead, 0, -sinf(radian) * lookAhead };
		DirectX::XMVECTOR BaseVN = DirectX::XMVector3Normalize(BaseV);

		// �G����v���C���[�Ɍ������x�N�g��
		DirectX::XMVECTOR PlayerV = { targetPos.x - enemyPos.x, 0, targetPos.z - enemyPos.z };
		float enemyToPlayerDistance = DirectX::XMVectorGetX(DirectX::XMVector3Length(PlayerV));
		DirectX::XMVECTOR PlayerVN = DirectX::XMVector3Normalize(PlayerV);
		
		DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(BaseVN, PlayerVN);

		float dot = 0.0f;
		DirectX::XMStoreFloat(&dot, Dot);

		


		if (enemyToPlayerDistance < targetMaxDistance)
		{
			enemyToPlayerDistance -= targetMinDistance;	// 1�܂ł̑傫���ł܂�߂�
			enemyToPlayerDistance /= (targetMaxDistance - targetMinDistance);
			enemyToPlayerDistance = 1.0f - enemyToPlayerDistance;

			DirectX::XMVECTOR velocityV = DirectX::XMLoadFloat3(&contextVelocity);
			DirectX::XMVECTOR velocityVN = DirectX::XMVector3Normalize(velocityV);
			DirectX::XMVECTOR VDot = DirectX::XMVector3Dot(BaseVN, velocityVN);
			float vDot = 0.0f;
			DirectX::XMStoreFloat(&vDot, VDot);

			dot = (1 - fabsf(dot)) + vDot * 0.2f;
		}
		
		if (dot < 0) dot = 0.0f;
		
		interest[index] = dot;
	}
}

// �댯�`�F�b�N
void EnemyContextBaseSlime::AccelerationCheck()
{
	// ���C�̊J�n�ʒu�͑�����菭����
	DirectX::XMFLOAT3 start = { position.x, position.y + stepOffset, position.z };

	for (int index = 0; index < numRays; index++)
	{
		danger[index] = 0;

		float radian = DirectX::XMConvertToRadians(rayDirections[index]);

		float endX = position.x + cosf(radian) * lookAhead;
		float endZ = position.z + (-sinf(radian)) * lookAhead;
		
		// ���C�̏I�_�ʒu�͈ړ���̈ʒu
		DirectX::XMFLOAT3 end = { endX, position.y + stepOffset, endZ };

		// ���C�L���X�g�ɂ��n�ʔ���
		HitResult hit;
		if (StageManager::Instance().RayCast(start, end, hit))
		{
			// �ǂɋ߂Â��� weight �̒l���傫���Ȃ�
			float weight = 1.0f - ((hit.distance - 1.0f) / lookAhead);

			if (danger[index] <= 1 * weight)
			{
				danger[index] = 1 * weight;
			}
			//danger[index] += 1 * weight;
		}

		// ���C�L���X�g�ɂ��n�ʔ���
		if (EnemyManager::Instance().RayCast(start, end, hit, id))
		{
			// �ǂɋ߂Â��� weight �̒l���傫���Ȃ�
			//float weight = 1.0f - (hit.distance / lookAhead);
			//danger[index] += 1 * weight;
			danger[index] = 1;
		}

		if (danger[index] > 1.0f) danger[index] = 1.0f;
	}
}

// �ŏI�I�Ȉړ��x�N�g���Z�o
void EnemyContextBaseSlime::CalcContextVec()
{
	float maxLength = 0.0f;
	for (int index = 0; index < numRays; index++)
	{
		if (maxLength < interest[index]) maxLength = interest[index];
	}

	DirectX::XMVECTOR AddV = DirectX::XMVectorZero();
	for (int index = 0; index < numRays; index++)
	{
		float radian = DirectX::XMConvertToRadians(rayDirections[index]);

		float dot = interest[index] - danger[index];

		dot /= maxLength;

		if (dot <= 0) continue;

		DirectX::XMVECTOR V = { cosf(radian) * lookAhead * dot, 0.0f, -sinf(radian) * lookAhead * dot };

		AddV = DirectX::XMVectorAdd(AddV, V);
	}
	DirectX::XMVECTOR AddVNormal = DirectX::XMVector3Normalize(AddV);
	DirectX::XMStoreFloat3(&contextVelocity, AddVNormal);
}

// �ǐՏI���`�F�b�N
void EnemyContextBaseSlime::EndChaseCheck()
{
	float offset = 0.01f;
	if (!(lastTargetPosition.x > position.x - offset && lastTargetPosition.x <= position.x + offset)) return;
	if (!(lastTargetPosition.y > position.y - offset && lastTargetPosition.y <= position.y + offset)) return;
	if (!(lastTargetPosition.z > position.z - offset && lastTargetPosition.z <= position.z + offset)) return;

	TransitionIdleState();
}

