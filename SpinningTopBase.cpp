#include "SpinningTopBase.h"
#include "Library/Timer.h"
#include "Library/ImGui/ConsoleData.h"
#include "Library/3D/DebugPrimitive.h"
#include "StageManager.h"

// �s��X�V����
void SpinningTopBase::UpdateTransform()
{
	// �T�C�Y�����s����쐬
	const float scaleFactor = model->scaleFactors[model->fbxUnit];
	DirectX::XMMATRIX C = DirectX::XMLoadFloat4x4(&model->coordinateSystemTransform[model->coordinateSystemIndex]) * DirectX::XMMatrixScaling(scaleFactor, scaleFactor, scaleFactor);

	// �X�P�[���s����쐬
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	// ��]�s����쐬
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(angle.x), DirectX::XMConvertToRadians(angle.y), DirectX::XMConvertToRadians(angle.z));
	// �ʒu�s����쐬
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	// �S�̍s���g�ݍ��킹�A���[���h�s����쐬
	DirectX::XMMATRIX W = C * S * R * T;
	// �v�Z�������[���h�s������o��
	DirectX::XMStoreFloat4x4(&transform, W);
}

// �_���[�W��^����
bool SpinningTopBase::ApplyDamage(int damage, float invincibleTime)
{
	if (damage <= 0) return false;

	if (health <= 0) return false;

	if (invincibleTimer > 0.0f) return false;

	// ���G���Ԑݒ�
	invincibleTimer = invincibleTime;

	// �_���[�W����
	health -= damage;

	// ���S�ʒm
	if (health <= 0)
	{
		OnDead();
	}
	// �_���[�W�ʒm
	else
	{
		OnDamaged();
	}

	// ���N��Ԃ��ύX�����ꍇ��true��Ԃ�
	return true;
}

// �Ռ���^����
void SpinningTopBase::AddImpulse(const DirectX::XMFLOAT3& impulse)
{
	// ���͂ɗ͂�������
	velocity.x += impulse.x;
	velocity.y += impulse.y;
	velocity.z += impulse.z;
}

// �ړ�����
void SpinningTopBase::Move(float vx, float vz, float speed)
{
	// �ړ������x�N�g����ݒ�
	moveVecX = vx;
	moveVecZ = vz;

	// �ő呬�x�ݒ�
	maxMoveSpeed = speed;
}

// ���񏈗�
void SpinningTopBase::Turn(float vx, float vz, float speed)
{
	speed *= Timer::Instance().DeltaTime();

	// �i�s�x�N�g�����[���x�N�g���̏ꍇ�͏�������K�v�Ȃ�
	float length = sqrtf(vx * vx + vz * vz);
	if (length < 0.001f)
	{
		return;
	}

	// �i�s�x�N�g����P�ʃx�N�g����
	vx /= length;
	vz /= length;

	// ���g�̉�]�l����O���������߂�
	float frontX = sinf(DirectX::XMConvertToRadians(angle.y));
	float frontZ = cosf(DirectX::XMConvertToRadians(angle.y));

	// ��]�p�����߂邽�߁A�Q�̒P�ʃx�N�g���̓��ς��v�Z����
	float dot = (frontX * vx) + (frontZ * vz);

	// ���ϒl��-1.0�`1.0�ŕ\������Ă���A�Q�̒P�ʃx�N�g���̊p�x��
	// �������ق�1.0�ɋ߂Â��Ƃ��������𗘗p���ĉ�]���x�𒲐�����
	float rot = 1.0f - dot;
	rot *= 60;
	if (rot > speed) rot = speed;


	// ���E������s�����߂ɂQ�̒P�ʃx�N�g���̊O�ς��v�Z����
	float cross = (frontX * vz) - (frontZ * vx);

	// 2D�O�ϒl�����̏ꍇ�����̏ꍇ�ɂ���č��E���肪�s����
	// ���E������s�����Ƃɂ���č��E��]���s��
	if (cross < 0.0f)
	{
		angle.y += rot;
	}
	else
	{
		angle.y -= rot;
	}
}

// �W�����v����
void SpinningTopBase::Jump(float speed)
{
	// ������̗͂�ݒ�
	velocity.y = speed;
}

void SpinningTopBase::UpdateVelocity()
{
	// �������͍X�V����
	UpdateVerticalVelocity();
	// �����ړ��X�V����
	UpdateVerticalMove();

	// �������͍X�V����
	UpdateHorizontalVelocity();
	// �����ړ��X�V����
	UpdateHorizontalMove();
}

// ���G���ԏ���
void SpinningTopBase::UpdateInvincibleTimer()
{
	if (invincibleTimer > 0.0f)
	{
		invincibleTimer -= Timer::Instance().DeltaTime();
	}
}

// �������͍X�V����
void SpinningTopBase::UpdateVerticalVelocity()
{
	// �d�͏���
	velocity.y += gravity * Timer::Instance().DeltaTime() * 60;
}

// �����ړ��X�V����
void SpinningTopBase::UpdateVerticalMove()
{
	// ���������̈ړ���
	float my = velocity.y * Timer::Instance().DeltaTime();

	// �L�����N�^�[��Y�������ƂȂ�@���x�N�g��
	DirectX::XMFLOAT3 normal = { 0,1,0 };

	slopeRate = 0.0f;

	// ������
	if (my < 0.0f)
	{
		// ���C�̊J�n�ʒu�͑�����菭����
		DirectX::XMFLOAT3 start = { position.x, position.y + stepOffset, position.z };
		// ���C�̏I�_�ʒu�͈ړ���̈ʒu
		DirectX::XMFLOAT3 end = { position.x, position.y + my, position.z };

		// ���C�L���X�g�ɂ��n�ʔ���
		HitResult hit;
		if (StageManager::Instance().RayCast(start, end, hit))
		{
			// �@���x�N�g���擾
			normal = hit.normal;

			// �X�Η��̌v�Z
			float normalLengthXZ = sqrtf(hit.normal.x * hit.normal.x + hit.normal.z * hit.normal.z);
			slopeRate = 1.0f - (hit.normal.y / (normalLengthXZ + hit.normal.y));

			// �n�ʂɐڒn���Ă���
			position.y = hit.position.y;

			// ���n����
			if (!isGround)
			{
				OnLanding();
			}
			isGround = true;
			velocity.y = 0;
		}
		else
		{
			// �󒆂ɕ����Ă���
			position.y += my;
			isGround = false;
		}
	}
	// �㏸��
	else if (my > 0.0f)
	{
		position.y += my;
		isGround = false;
	}
}

// �������͍X�V����
void SpinningTopBase::UpdateHorizontalVelocity()
{
	// XZ���ʂ̑��͂���������
	float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
	if (length > 0.0f)
	{
		// ���C��
		float friction = this->friction * Timer::Instance().DeltaTime() * 60.0f;
		// �󒆂ɂ���Ƃ��͖��C�͂����炷
		if (!isGround) friction *= airControl;

		// ���C�ɂ�鉡�����̌�������
		if (length > friction)
		{
			// �P�ʃx�N�g����
			float vx = velocity.x / length;
			float vz = velocity.z / length;
			velocity.x -= vx * friction;
			velocity.z -= vz * friction;

		}
		// �������̑��͂����C�͈ȉ��ɂȂ����̂ő��͂𖳌���
		else
		{
			velocity.x = 0.0f;
			velocity.z = 0.0f;
		}
	}

	// XZ���ʂ̑��͂���������
	if (length <= maxMoveSpeed)
	{
		// �ړ��x�N�g�����[���x�N�g���łȂ��Ȃ��������
		float moveVecLength = sqrtf(moveVecX * moveVecX + moveVecZ * moveVecZ);
		if (moveVecLength > 0.0f)
		{
			// ������
			float acceleration = this->acceleration * Timer::Instance().DeltaTime() * 60.0f;
			// �󒆂ɂ���Ƃ��͖��C�͂����炷
			if (!isGround) acceleration *= airControl;
			// �ړ��x�N�g���ɂ���������
			velocity.x += moveVecX * acceleration;
			velocity.z += moveVecZ * acceleration;
			// �ő呬�x����
			float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
			if (length > maxMoveSpeed)
			{
				float vx = velocity.x / length;
				float vz = velocity.z / length;
				velocity.x = vx * maxMoveSpeed;
				velocity.z = vz * maxMoveSpeed;
			}

			// �����ŃK�^�K�^���Ȃ��悤�ɂ���
			if (isGround && slopeRate > 0.0f)
			{
				velocity.y -= length * slopeRate * Timer::Instance().DeltaTime() * 60.0f;
			}
		}
	}
	// �ő呬�x����
	length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
	if (length > maxMoveSpeed)
	{
		float vx = velocity.x / length;
		float vz = velocity.z / length;
		velocity.x = vx * maxMoveSpeed;
		velocity.z = vz * maxMoveSpeed;
	}

	// �ړ��x�N�g���[��
	moveVecX = 0;
	moveVecZ = 0;
}

// �����ړ��X�V����
void SpinningTopBase::UpdateHorizontalMove()
{
	// �������͗ʌv�Z
	float velocityLengthXZ = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
	if (velocityLengthXZ > 0.0f)
	{
		// �����ړ��l
		float mx = velocity.x * Timer::Instance().DeltaTime();
		float mz = velocity.z * Timer::Instance().DeltaTime();

		// ���C�̊J�n�ʒu�ƏI�_�ʒu
		DirectX::XMFLOAT3 start = { position.x, position.y + stepOffset, position.z };
		DirectX::XMFLOAT3 end = { position.x + mx , position.y + stepOffset, position.z + mz };

		// ���C�L���X�g�ɂ��ǔ���
		HitResult hit;
		if (StageManager::Instance().RayCast(start, end, hit))
		{
			// �ǂ܂ł̃x�N�g��
			DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);
			DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end);
			DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);

			// �ǂ̖@��
			DirectX::XMVECTOR Normal = DirectX::XMLoadFloat3(&hit.normal);
			DirectX::XMVectorScale(Normal, 1.1f);

			// ���˃x�N�g����@���Ɏˉe
			DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(DirectX::XMVectorNegate(Vec), Normal);

			// �␳�ʒu�̌v�Z
			DirectX::XMVECTOR CollectPosition = DirectX::XMVectorMultiplyAdd(Normal, Dot, End);
			DirectX::XMFLOAT3 collectPosition;
			DirectX::XMStoreFloat3(&collectPosition, CollectPosition);

			// �ǂ�������փ��C�L���X�g
			HitResult hit2;
			if (!StageManager::Instance().RayCast(hit.position, collectPosition, hit2))
			{
				// �ǂ�������ŕǂɂ�����Ȃ�������␳�ʒu�Ɉړ�
				position.x = collectPosition.x;
				position.z = collectPosition.z;
			}
			else
			{
				// �ǂ܂ł̃x�N�g��
				DirectX::XMVECTOR Start2 = DirectX::XMLoadFloat3(&hit.position);
				DirectX::XMVECTOR End2 = DirectX::XMLoadFloat3(&collectPosition);
				DirectX::XMVECTOR Vec2 = DirectX::XMVectorSubtract(End2, Start2);

				// �ǂ̖@��
				DirectX::XMVECTOR Normal2 = DirectX::XMLoadFloat3(&hit2.normal);
				DirectX::XMVectorScale(Normal2, 1.1f);

				// ���˃x�N�g����@���Ɏˉe
				DirectX::XMVECTOR Dot2 = DirectX::XMVector3Dot(DirectX::XMVectorNegate(Vec2), Normal2);

				// �␳�ʒu�̌v�Z
				DirectX::XMVECTOR CollectPosition2 = DirectX::XMVectorMultiplyAdd(Normal2, Dot2, End2);
				DirectX::XMFLOAT3 collectPosition2;
				DirectX::XMStoreFloat3(&collectPosition2, CollectPosition2);

				// �ǂ�������փ��C�L���X�g
				HitResult hit3;
				if (!StageManager::Instance().RayCast(hit2.position, collectPosition2, hit3))
				{
					// �ǂ�������ŕǂɂ�����Ȃ�������␳�ʒu�Ɉړ�
					position.x = collectPosition2.x;
					position.z = collectPosition2.z;
				}
				else
				{
					// �ǂ܂ł̃x�N�g��
					DirectX::XMVECTOR Start3 = DirectX::XMLoadFloat3(&hit2.position);
					DirectX::XMVECTOR End3 = DirectX::XMLoadFloat3(&collectPosition2);
					DirectX::XMVECTOR Vec3 = DirectX::XMVectorSubtract(End3, Start3);

					// �ǂ̖@��
					DirectX::XMVECTOR Normal3 = DirectX::XMLoadFloat3(&hit3.normal);
					DirectX::XMVectorScale(Normal3, 1.1f);

					// ���˃x�N�g����@���Ɏˉe
					DirectX::XMVECTOR Dot3 = DirectX::XMVector3Dot(DirectX::XMVectorNegate(Vec3), Normal3);

					// �␳�ʒu�̌v�Z
					DirectX::XMVECTOR CollectPosition3 = DirectX::XMVectorMultiplyAdd(Normal3, Dot3, End3);
					DirectX::XMFLOAT3 collectPosition3;
					DirectX::XMStoreFloat3(&collectPosition3, CollectPosition3);

					// �ǂ�������փ��C�L���X�g
					HitResult hit4;
					if (!StageManager::Instance().RayCast(hit3.position, collectPosition3, hit4))
					{
						// �ǂ�������ŕǂɂ�����Ȃ�������␳�ʒu�Ɉړ�
						position.x = collectPosition3.x;
						position.z = collectPosition3.z;
					}
					else
					{
						position.x = hit4.position.x;
						position.z = hit4.position.z;
					}
				}

				//position.x = hit2.position.x;
				//position.z = hit2.position.z;
			}
		}
		// �ǂɏՓ˂��Ȃ������ꍇ
		else
		{
			// �ړ�
			position.x += mx;
			position.z += mz;
		}
	}
}

//==========================================================================
//
//�@�G�t�F�N�g����
// 
//==========================================================================

// �G�t�F�N�g�X�V����
void SpinningTopBase::UpdateEffect()
{
	using namespace DirectX;

	// �T�C�Y�̏C��
	const float scaleFactor = model->scaleFactors[model->fbxUnit];
	DirectX::XMMATRIX C = DirectX::XMLoadFloat4x4(&model->coordinateSystemTransform[model->coordinateSystemIndex]) * DirectX::XMMatrixScaling(scaleFactor, scaleFactor, scaleFactor);

	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(XMConvertToRadians(angle.x), XMConvertToRadians(angle.y), XMConvertToRadians(angle.z));
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	DirectX::XMFLOAT4X4 world;
	DirectX::XMStoreFloat4x4(&world, C * S * R * T);

	int effectCount = model->animationClips.at(currentAnimationIndex).animEffects.size();

	for (int i = 0; i < effectCount; i++)
	{
		// ���ɍĐ�����Ă���Ȃ�Đ����Ȃ�
		if (model->animationClips.at(currentAnimationIndex).animEffects.at(i).isPlay) continue;

		int startFrame = model->animationClips.at(currentAnimationIndex).animEffects.at(i).startFrame;
		int endFrame = model->animationClips.at(currentAnimationIndex).animEffects.at(i).endFrame;

		//if (currentFrameInt < startFrame || currentFrameInt > endFrame) continue;
		if (currentKeyFrame != startFrame) continue;

		model->animationClips.at(currentAnimationIndex).animEffects.at(i).isPlay = true;

		EffectType type = model->animationClips.at(currentAnimationIndex).animEffects.at(i).effectType;
		float effectScale = model->animationClips.at(currentAnimationIndex).animEffects.at(i).scale;

		DirectX::XMFLOAT3 effectPosition = model->animationClips.at(currentAnimationIndex).animEffects.at(i).position;
		DirectX::XMFLOAT3 effectAngle = model->animationClips.at(currentAnimationIndex).animEffects.at(i).angle;

		// ��]��̃R���W�����ʒu���v�Z
		DirectX::XMVECTOR effectPosVec = DirectX::XMLoadFloat3(&effectPosition);
		effectPosVec = DirectX::XMVector3TransformCoord(effectPosVec, R); // ��]��K�p
		DirectX::XMStoreFloat3(&effectPosition, effectPosVec);

		T = DirectX::XMMatrixTranslation(
			position.x + effectPosition.x,
			position.y + effectPosition.y,
			position.z + effectPosition.z
		);

		DirectX::XMStoreFloat4x4(&world, C * S * R * T);

		effectPosition =
		{
			world._41,
			world._42,
			world._43,
		};

		effectAngle.x += angle.x;
		effectAngle.y += angle.y;
		effectAngle.z += angle.z;

		Effect::Instance().Play(type, effectPosition, effectAngle, effectScale);
	}
}

// �G�t�F�N�g�t���O�N���A
void SpinningTopBase::ClearEffectFlag()
{
	int effectCount = model->animationClips.at(currentAnimationIndex).animEffects.size();

	for (int i = 0; i < effectCount; i++)
	{
		model->animationClips.at(currentAnimationIndex).animEffects.at(i).isPlay = false;
	}
}

//==========================================================================
//
//�@���ʉ�����
// 
//==========================================================================

// ���ʉ��X�V����
void SpinningTopBase::UpdateSE()
{
	int seCount = model->animationClips.at(currentAnimationIndex).animSEs.size();

	for (int i = 0; i < seCount; i++)
	{
		// ���ɍĐ�����Ă���Ȃ�Đ����Ȃ�
		if (model->animationClips.at(currentAnimationIndex).animSEs.at(i).isPlay) continue;

		int startFrame = model->animationClips.at(currentAnimationIndex).animSEs.at(i).startFrame;
		int endFrame = model->animationClips.at(currentAnimationIndex).animSEs.at(i).endFrame;

		//if (currentFrameInt < startFrame || currentFrameInt > endFrame) continue;
		if (currentKeyFrame != startFrame) continue;

		model->animationClips.at(currentAnimationIndex).animSEs.at(i).isPlay = true;

		MUSIC_LABEL type = model->animationClips.at(currentAnimationIndex).animSEs.at(i).musicType;

		AudioManager::Instance().PlayMusic(static_cast<int>(type));
	}
}

// ���ʉ��t���O�N���A
void SpinningTopBase::ClearSEFlag()
{
	int seCount = model->animationClips.at(currentAnimationIndex).animSEs.size();

	for (int i = 0; i < seCount; i++)
	{
		model->animationClips.at(currentAnimationIndex).animSEs.at(i).isPlay = false;
	}
}

//==========================================================================
//
//�@�A�j���[�V��������
// 
//==========================================================================

// �A�j���[�V�����Đ�
void SpinningTopBase::PlayAnimation(int index, bool loop)
{
	currentAnimationIndex = index;
	currentAnimationSeconds = 0.0f;

	animationLoopFlag = loop;
	animationEndFlag = false;

	animationBlendTime = 0.0f;
	animationBlendSeconds = 0.2f;
}

// �A�j���[�V�����X�V
void SpinningTopBase::UpdateAnimation()
{
	// �ŏI�t���[������
	if (animationEndFlag) return;


	if (blendAnimationIndex != -1)
	{
		// �u�����h���̌v�Z
		float blendRate = 1.0f;
		if (animationBlendTime < animationBlendSeconds)
		{
			animationBlendTime += Timer::Instance().DeltaTime();
			if (animationBlendTime >= animationBlendSeconds)
			{
				animationBlendTime = animationBlendSeconds;
			}
			blendRate = animationBlendTime / animationBlendSeconds;
			blendRate *= blendRate;

			Animation& blendAnimation = model->animationClips.at(blendAnimationIndex);
			int blendAnimationMaxKeyFrame = blendAnimation.sequence.size() - 1;
			Animation& currentAnimation = model->animationClips.at(currentAnimationIndex);

			Animation::KeyFrame keyframe;
			const Animation::KeyFrame* keyframes[2] =
			{
				&blendAnimation.sequence.at(blendAnimationMaxKeyFrame),
				&currentAnimation.sequence.at(0)
			};
			model->BlendAnimations(keyframes, blendRate, keyFrame);
			model->UpdateAnimation(keyFrame);

			return;
		}
		else
		{
			blendAnimationIndex = -1;
		}
	}


	Animation& animation = model->animationClips.at(currentAnimationIndex);

	currentKeyFrame = static_cast<int>(currentAnimationSeconds * animation.samplingRate);

	currentAnimationSeconds += Timer::Instance().DeltaTime();
	// ���݂̃t���[�����ő�t���[���𒴂��Ă�����
	if (currentAnimationSeconds >= animation.secondsLength)
	{
		if (animationLoopFlag)
		{
			// �擪�b����
			currentKeyFrame = 0;
			currentAnimationSeconds -= animation.secondsLength;
		}
		else
		{
			// �ŏI�b���Œ�~
			currentKeyFrame = animation.sequence.size() - 1;
			currentAnimationSeconds = animation.secondsLength;
			animationEndFlag = true;
		}
		// �G�t�F�N�g�t���O�̃N���A
		ClearEffectFlag();
		// ���ʉ��t���O�̃N���A
		ClearSEFlag();
	}

	// keyFrame�ݒ�
	keyFrame = animation.sequence.at(currentKeyFrame);
}


//==========================================================================
//
//�@�f�o�b�O�v���~�e�B�u�`��
// 
//==========================================================================

void SpinningTopBase::DrawDebugPrimitive()
{
	// --- �R���W�����`�� ---
	if (model && model->animationClips.size() > 0)
	{
		// sphere
		int collisionCount = model->animationClips.at(currentAnimationIndex).spheres.size();

		for (int i = 0; i < collisionCount; i++)
		{
			int startFrame = model->animationClips.at(currentAnimationIndex).spheres.at(i).startFrame;
			int endFrame = model->animationClips.at(currentAnimationIndex).spheres.at(i).endFrame;

			if (currentKeyFrame < startFrame || currentKeyFrame > endFrame) continue;

			float radius = model->animationClips.at(currentAnimationIndex).spheres.at(i).radius;
			DirectX::XMFLOAT4 color = model->animationClips.at(currentAnimationIndex).spheres.at(i).color;

			DirectX::XMFLOAT3 collisionPosition = { 0,0,0 };


			if (model->animationClips.at(currentAnimationIndex).spheres.at(i).bindBoneName != "")
			{
				Animation::KeyFrame& keyframe = model->animationClips.at(currentAnimationIndex).sequence.at(currentKeyFrame);

				int nodeCount = keyframe.nodes.size();
				for (int nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
				{
					Animation::KeyFrame::Node& node = keyframe.nodes.at(nodeIndex);

					// �{�[���̃m�[�h�Ɠ������O�Ȃ珈������
					if (node.name != model->animationClips.at(currentAnimationIndex).spheres.at(i).bindBoneName) continue;

					DirectX::XMMATRIX T = DirectX::XMLoadFloat4x4(&transform);
					DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&node.globalTransform);

					DirectX::XMMATRIX W = M * T;
					DirectX::XMFLOAT4X4 w;
					DirectX::XMStoreFloat4x4(&w, W);

					collisionPosition = { w._41, w._42, w._43 };
					break;
				}
			}
			else
			{
				// �T�C�Y�̏C��
				const float scaleFactor = model->scaleFactors[model->fbxUnit];
				DirectX::XMMATRIX C = DirectX::XMLoadFloat4x4(&model->coordinateSystemTransform[model->coordinateSystemIndex]) * DirectX::XMMatrixScaling(scaleFactor, scaleFactor, scaleFactor);

				DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
				DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(angle.x), DirectX::XMConvertToRadians(angle.y), DirectX::XMConvertToRadians(angle.z));
				DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

				DirectX::XMFLOAT4X4 world;

				collisionPosition = model->animationClips.at(currentAnimationIndex).spheres.at(i).position;

				// ��]��̃R���W�����ʒu���v�Z
				DirectX::XMVECTOR collisionPosVec = DirectX::XMLoadFloat3(&collisionPosition);
				collisionPosVec = DirectX::XMVector3TransformCoord(collisionPosVec, R); // ��]��K�p
				DirectX::XMStoreFloat3(&collisionPosition, collisionPosVec);

				T = DirectX::XMMatrixTranslation(
					position.x + collisionPosition.x,
					position.y + collisionPosition.y,
					position.z + collisionPosition.z
				);

				DirectX::XMStoreFloat4x4(&world, C * S * R * T);

				collisionPosition = { world._41, world._42, world._43 };
			}

			DebugPrimitive::Instance().AddSphere(collisionPosition, radius, color);
		}
	}
}
