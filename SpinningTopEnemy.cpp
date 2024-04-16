#include "SpinningTopEnemy.h"
#include "SpinningTopEnemyManager.h"
#include "Collision.h"

// ���C�L���X�g
bool SpinningTopEnemy::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	return Collision::IntersectRayVsModel(start, end, model.get(), transform, hit);
}

// �j��
void SpinningTopEnemy::Destroy()
{
	SpinningTopEnemyManager::Instance().Remove(this);
}

bool SpinningTopEnemy::GetBTreeJudge(const int _kind)
{
	KIND kind = (KIND)_kind;
	switch (kind)
	{
	case KIND::WANDER:
		return true;
	}

	return true;
}

IBTree::STATE SpinningTopEnemy::ActBTree(const int _kind)
{
	KIND kind = (KIND)_kind;
	switch (kind)
	{
	case KIND::WANDER:
		DirectX::XMFLOAT3 ang = GetAngle();
		ang.y+=2;
		SetAngle(ang);
		return  IBTree::STATE::Run;
		break;
	case KIND::IDLE:
		return  IBTree::STATE::Complete;
		break;
	case KIND::SEEK:
		// �x�N�^�[�쐬
		DirectX::XMVECTOR POSITION = DirectX::XMLoadFloat3(&GetPosition());
		DirectX::XMVECTOR TARGET = DirectX::XMLoadFloat3(&targetPosition);
		DirectX::XMVECTOR VELOCITY = DirectX::XMLoadFloat3(&velocity);
		DirectX::XMVECTOR DSERIRED_VELOCITY;
		DirectX::XMVECTOR STEERING;

		// �^�[�Q�b�g�ւ̃x�N�g�������߂�
		DirectX::XMVECTOR ToTarget = DirectX::XMVectorSubtract(TARGET, POSITION);
		ToTarget = DirectX::XMVector3Normalize(ToTarget);					// ���K��
		DSERIRED_VELOCITY = DirectX::XMVectorScale(ToTarget, maxMoveSpeed);	// �X�P�[�����O

		// �X�e�A�����O�͂̌v�Z
		STEERING = DirectX::XMVectorSubtract(DSERIRED_VELOCITY, VELOCITY);
		DirectX::XMVECTOR LENGTH = DirectX::XMVector3LengthSq(STEERING);
		float length;
		DirectX::XMStoreFloat(&length, LENGTH);

		if (length > steeringMaxForce)
		{
			STEERING = DirectX::XMVector3Normalize(STEERING);				// ���K��
			STEERING = DirectX::XMVectorScale(STEERING, steeringMaxForce);	// �X�P�[�����O
		}

		DirectX::XMStoreFloat3(&steeringForce, STEERING);

		velocity.x += steeringForce.x;
		velocity.z += steeringForce.z;

		Move(velocity.x, velocity.z, maxMoveSpeed);

		return  IBTree::STATE::Run;
		break;
	case KIND::ARRIVAL:
	{
		// �x�N�^�[�쐬
		DirectX::XMVECTOR POSITION = DirectX::XMLoadFloat3(&GetPosition());
		DirectX::XMVECTOR TARGET = DirectX::XMLoadFloat3(&targetPosition);
		DirectX::XMVECTOR VELOCITY = DirectX::XMLoadFloat3(&velocity);
		DirectX::XMVECTOR DSERIRED_VELOCITY;
		DirectX::XMVECTOR STEERING;

		// �^�[�Q�b�g�ւ̃x�N�g�������߂�
		DirectX::XMVECTOR ToTarget = DirectX::XMVectorSubtract(TARGET, POSITION);
		DirectX::XMVECTOR TargetDistance = DirectX::XMVector3Length(ToTarget);
		float targetDistance;
		DirectX::XMStoreFloat(&targetDistance, TargetDistance);

		// slow �G���A
		if (targetDistance > slowingArea)
		{
			ToTarget = DirectX::XMVector3Normalize(ToTarget);					// ���K��
			DSERIRED_VELOCITY = DirectX::XMVectorScale(ToTarget, maxMoveSpeed);	// �X�P�[�����O
		}
		else
		{
			ToTarget = DirectX::XMVector3Normalize(ToTarget);					// ���K��
			DSERIRED_VELOCITY = DirectX::XMVectorScale(ToTarget, maxMoveSpeed);	// �X�P�[�����O
			DSERIRED_VELOCITY = DirectX::XMVectorScale(DSERIRED_VELOCITY, targetDistance / slowingArea);
		}
		// �X�e�A�����O�͂̌v�Z
		STEERING = DirectX::XMVectorSubtract(DSERIRED_VELOCITY, VELOCITY);
		DirectX::XMVECTOR LENGTH = DirectX::XMVector3LengthSq(STEERING);
		float length;
		DirectX::XMStoreFloat(&length, LENGTH);

		if (length > steeringMaxForce)
		{
			STEERING = DirectX::XMVector3Normalize(STEERING);				// ���K��
			STEERING = DirectX::XMVectorScale(STEERING, steeringMaxForce);	// �X�P�[�����O
		}

		DirectX::XMStoreFloat3(&steeringForce, STEERING);

		velocity.x += steeringForce.x;
		velocity.z += steeringForce.z;

		Move(velocity.x, velocity.z, maxMoveSpeed);

		return  IBTree::STATE::Run;

		break;
	}
	}

	return IBTree::STATE::Complete;
}
