#include "Player.h"
#include "Library/Graphics/Graphics.h"
#include "Library/Timer.h"
#include "Library/ImGui/Include/imgui.h"
#include "Library/ImGui/ConsoleData.h"
#include "Library/Input/InputManager.h"
#include "Library/3D/Camera.h"
#include "Library/3D/DebugPrimitive.h"
#include "EnemyManager.h"
#include "Collision.h"
#include "ProjectileStraight.h"
#include "ProjectileHoming.h"
#include "Library/MemoryLeak.h"
#include "Library/Effekseer/Effect.h"
#include "Library/Audio/AudioManager.h"
#include "DamageTextManager.h"
#include "Library/3D/ResourceManager.h"

//==========================================================================
//
//�@�������E�I����
// 
//==========================================================================

// �R���X�g���N�^
Player::Player()
{
	//model = ResourceManager::Instance().LoadModelResource("Data/Fbx/Chara/character.fbx");

	model = std::make_unique<SkinnedMesh>("Data/Fbx/Chara/character.fbx");

	// �ҋ@�X�e�[�g�֑J��
	TransitionIdleState();
}

// �f�X�g���N�^
Player::~Player()
{
}


//==========================================================================
//
//�@�X�V����
// 
//==========================================================================

// �X�V����
void Player::Update()
{
	// �X�e�[�g���Ƃ̏���
	switch (state)
	{
	case Player::State::Idle:
		UpdateIdleState();
		break;
	case Player::State::Move:
		UpdateMoveState();
		break;
	case Player::State::Jump:
		UpdateJumpState();
		break;
	case Player::State::Attack:
		UpdateAttackState();
		break;
	case Player::State::Victory:
		UpdateVictoryUpdate();
		break;
	}
	
	// ���͍X�V����
	UpdateVelocity();

	// ----------------- �v���C���[�ƓG�Ƃ̏Փˏ��� -----------------
	CollisionPlayerVsEnemies();
	// �e�ۂƓG�̏Փˏ���
	CollisionProjectilesVsEnemies();
	// �A�j���[�V�����ƓG�̏Փˏ���
	CollisionNodeVsEnemies();


	// �e�ۍX�V����
	projectileManager.Update();

	UpdateAnimation();

	UpdateEffect();

	UpdateSE();

	// ------------------- �I�u�W�F�N�g�s����X�V -------------------
	UpdateTransform();
}

// �ړ����͏���
bool Player::InputMove()
{
	// �i�s�x�N�g���擾
	DirectX::XMFLOAT3 moveVec = GetMoveVec();
	// �ړ�����
	Move(moveVec.x, moveVec.z, moveSpeed);
	// ���񏈗�
	Turn(moveVec.x, moveVec.z, turnSpeed);

	// �i�s�x�N�g�����[���x�N�g���łȂ��ꍇ�͓��͂��ꂽ
	return moveVec.x != 0 || moveVec.z != 0;
}

DirectX::XMFLOAT3 Player::GetMoveVec() const
{
	// --- ���͏��� ---
	InputManager& inputManager = InputManager::Instance();

	float ax = 0;
	float ay = 0;

	if (inputManager.GetKeyPress(DirectX::Keyboard::A)) ax--;
	if (inputManager.GetKeyPress(DirectX::Keyboard::D)) ax++;
	if (inputManager.GetKeyPress(DirectX::Keyboard::W)) ay++;
	if (inputManager.GetKeyPress(DirectX::Keyboard::S)) ay--;

	// �J���������ƃX�e�B�b�N�̓��͒l�ɂ���Đi�s�������v�Z����
	Camera& camera = Camera::Instance();
	const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
	const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

	// �ړ��x�N�g����XZ���ʂɐ����ȃx�N�g���ɂȂ�悤�ɂ���

	// �J�����E�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
	float cameraRightX = cameraRight.x;
	float cameraRightZ = cameraRight.z;
	float cameraRightLength = sqrtf(cameraRight.x * cameraRight.x + cameraRight.y * cameraRight.y + cameraRight.z * cameraRight.z);
	if (cameraRightLength > 0.0f)
	{
		// �P�ʃx�N�g����
		cameraRightX /= cameraRightLength;
		cameraRightZ /= cameraRightLength;
	}

	// �J�����O�����x�N�g����XZ�P�ʃx�N�g���ɕϊ�
	float cameraFrontX = cameraFront.x;
	float cameraFrontZ = cameraFront.z;
	float cameraFrontLength = sqrtf(cameraFront.x * cameraFront.x + cameraFront.y * cameraFront.y + cameraFront.z * cameraFront.z);
	if (cameraFrontLength > 0.0f)
	{
		// �P�ʃx�N�g����
		cameraFrontX /= cameraFrontLength;
		cameraFrontZ /= cameraFrontLength;
	}

	// �X�e�B�b�N�̐������͒l���J�����E�����ɔ��f���A
	// �X�e�B�b�N�̐������͒l���J�����O�����ɔ��f���A
	// �i�s�x�N�g�����v�Z����
	DirectX::XMFLOAT3 vec;
	vec.x = ax * cameraRightX + ay * cameraFrontX;
	vec.z = ax * cameraRightZ + ay * cameraFrontZ;
	// Y�������ɂ͈ړ����Ȃ�
	vec.y = 0.0f;

	return vec;
}

bool Player::InputJump()
{
	InputManager& input = InputManager::Instance();
	if (input.GetKeyPressed(Keyboard::Space))
	{
		// �W�����v�񐔐���
		if (jumpCount < jumpLimit)
		{
			// �W�����v
			jumpCount++;
			Jump(jumpSpeed);

			return true;
		}
	}
	return false;
}

// �U�����͏���
bool Player::InputAttack()
{
	InputManager& input = InputManager::Instance();
	if (input.GetMousePressed(MOUSEBUTTON_STATE::leftButton))
	{
		return true;
	}

	return false;
}

// ���n����
void Player::OnLanding()
{
	jumpCount = 0;

	// ���n�X�e�[�g�֑J��
	TransitionIdleState();
}

// �e�ۓ��͏���
void Player::InputProjectile()
{
	InputManager& inputManager = InputManager::Instance();

	// ���i�e�۔���
	if (inputManager.GetKeyPressed(Keyboard::Z))
	{
		// �O����
		DirectX::XMFLOAT3 dir;
		dir.x = sinf(DirectX::XMConvertToRadians(angle.y));
		dir.y = 0.0f;
		dir.z = cosf(DirectX::XMConvertToRadians(angle.y));
		// ���ˈʒu(�v���C���[�̍�������)
		DirectX::XMFLOAT3 pos;
		pos.x = position.x;
		pos.y = position.y + height * 0.5f;
		pos.z = position.z;
		// ����
		ProjectileStraight* projectile = new_ ProjectileStraight(&projectileManager);
		projectile->Launch(dir, pos);
	}

	// �ǔ��e�۔���
	if (inputManager.GetKeyPressed(Keyboard::X))
	{
		// �O����
		DirectX::XMFLOAT3 dir;
		dir.x = sinf(DirectX::XMConvertToRadians(angle.y));
		dir.y = 0.0f;
		dir.z = cosf(DirectX::XMConvertToRadians(angle.y));
		// ���ˈʒu(�v���C���[�̍�������)
		DirectX::XMFLOAT3 pos;
		pos.x = position.x;
		pos.y = position.y + height * 0.5f;
		pos.z = position.z;
		// �^�[�Q�b�g�i�f�t�H���g�ł̓v���C���[�̑O���j
		DirectX::XMFLOAT3 target;
		target.x = pos.x + dir.x * 1000.0f;
		target.y = pos.y + dir.y * 1000.0f;
		target.z = pos.z + dir.z * 1000.0f;

		// ��ԋ߂��̓G���^�[�Q�b�g�ɂ���
		float dist = FLT_MAX;
		EnemyManager& enemyManager = EnemyManager::Instance();
		int enemyCount = enemyManager.GetEnemyCount();
		for (int i = 0; i < enemyCount; i++)
		{
			// �G�Ƃ̋�������
			Enemy* enemy = EnemyManager::Instance().GetEnemy(i);
			DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
			DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
			DirectX::XMVECTOR V = DirectX::XMVectorSubtract(E, P);
			DirectX::XMVECTOR D = DirectX::XMVector3LengthSq(V);
			float d;
			DirectX::XMStoreFloat(&d, D);
			if (d < dist)
			{
				dist = d;
				target = enemy->GetPosition();
				target.y += enemy->GetHeight() * 0.5f;
			}
		}

		// ����
		ProjectileHoming* projectile = new_ ProjectileHoming(&projectileManager);
		projectile->Launch(dir, pos, target);
	}
}

// �v���C���[�ƃG�l�~�[�̂��炢����̏Փ�
void Player::CollisionBodyPlayerVsEnemies()
{

}

// �v���C���[�ƃG�l�~�[�̏Փˏ���
void Player::CollisionPlayerVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	enemyManager.SortPlaySortEnemiesByDistanceToPlayerEnemy(GetPosition());

	// �S�Ă̓G�Ƒ�������ŏՓ˔���
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < enemyCount; i++)
	{
		Enemy* enemy = enemyManager.GetEnemy(i);

		// 
		if (enemy->model->skeletonSpheres.size() > 0)
		{
			for (auto skeletonSphere : enemy->model->skeletonSpheres)
			{

				DirectX::XMFLOAT3 collisionPosition = { 0,0,0 };
				float skeletonRadius = skeletonSphere.radius;

				if (skeletonSphere.name != "")
				{
					// �{�[�����Ƃ̏���
					Animation::KeyFrame& keyframe = enemy->model->animationClips.at(enemy->currentAnimationIndex).sequence.at(enemy->currentKeyFrame);

					int nodeCount = keyframe.nodes.size();
					for (int nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
					{
						Animation::KeyFrame::Node& node = keyframe.nodes.at(nodeIndex);

						// �{�[���̃m�[�h�Ɠ������O�Ȃ珈������
						if (node.name != skeletonSphere.name) continue;

						DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&node.globalTransform);

						DirectX::XMMATRIX T = DirectX::XMLoadFloat4x4(&enemy->GetTransform());

						DirectX::XMMATRIX W = M * T;
						DirectX::XMFLOAT4X4 w;
						DirectX::XMStoreFloat4x4(&w, W);

						collisionPosition = { w._41, w._42, w._43 };
						break;
					}
				}

				// �Փˏ���
				DirectX::XMFLOAT3 outPosition;
				if (Collision::IntersectSphereVsSphere(
					collisionPosition,
					skeletonRadius,
					position,
					radius,
					outPosition)
					)
				{
					// �����o����̈ʒu�ݒ�
					SetPosition(outPosition);
				}
			}
		}
		else
		{
			DirectX::XMFLOAT3 outPosition;
			if (Collision::IntersectCylinderVsCylinder(
				enemy->GetPosition(),
				enemy->GetRadius(),
				enemy->GetHeight(),
				position,
				radius,
				height,
				outPosition)
				)
			{
				DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
				DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
				DirectX::XMVECTOR V = DirectX::XMVectorSubtract(P, E);
				DirectX::XMVECTOR N = DirectX::XMVector3Normalize(V);
				DirectX::XMFLOAT3 n;
				DirectX::XMStoreFloat3(&n, N);

				if (n.y > 0.5f)
				{
					velocity.y = 0.0f;
					Jump(jumpSpeed * 0.5f);
				}
				else
				{
					// �����o����̈ʒu�ݒ�
					SetPosition(outPosition);
				}
			}
		}		
	}
}

// �e�ۂƓG�̏Փˏ���
void Player::CollisionProjectilesVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	// �S�Ă̒e�ۂƑS�Ă̓G�𑍓�����ŏՓˏ���
	int projectileCount = projectileManager.GetProjectileCount();
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < projectileCount; i++)
	{
		Projectile* projectile = projectileManager.GetProjectile(i);

		for (int j = 0; j < enemyCount; j++)
		{
			Enemy* enemy = enemyManager.GetEnemy(j);

			// �Փˏ���
			DirectX::XMFLOAT3 outPosition;
			if (Collision::IntersectSphereVsCylinder(
				projectile->GetPosition(),
				projectile->GetRadius(),
				enemy->GetPosition(),
				enemy->GetRadius(),
				enemy->GetHeight(),
				outPosition
			))
			{
				// �_���[�W��^����
				if (enemy->ApplyDamage(1, 0.5f))
				{
					// ������΂�
					DirectX::XMFLOAT3 impulse;
					const float power = 10.0f;
					DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&projectile->GetPosition());
					DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
					DirectX::XMVECTOR V = DirectX::XMVectorSubtract(E, P);
					DirectX::XMVECTOR N = DirectX::XMVector3Normalize(V);
					DirectX::XMStoreFloat3(&impulse, N);
					impulse.x *= power;
					//impulse.y *= power;
					impulse.y = power*0.5f;
					impulse.z *= power;

					enemy->AddImpulse(impulse);

					// �q�b�g�G�t�F�N�g�Đ�
					DirectX::XMFLOAT3 e = enemy->GetPosition();
					e.y += enemy->GetHeight() * 0.5f;
					Effect::Instance().Play(EffectType::Hit1, e, {0,0,0});

					// �e�۔j��
					projectile->Destroy();

					// �_���[�W�\��
					int damage = 1;
					std::wstring damageString = std::to_wstring(damage);
					const TCHAR* damageTChar = damageString.c_str();
					DamageText* damageText = new_ DamageText(e, damageTChar);
					DamageTextManager::Instance().Register(damageText);
				}
			}
		}
	}
}

// �A�j���[�V�����ƃG�l�~�[�̏Փˏ���
void Player::CollisionNodeVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	// sphere
	int collisionCount = model->animationClips.at(currentAnimationIndex).spheres.size();
	int enemyCount = enemyManager.GetEnemyCount();

	for (int i = 0; i < collisionCount; i++)
	{
		int startFrame = model->animationClips.at(currentAnimationIndex).spheres.at(i).startFrame;
		int endFrame = model->animationClips.at(currentAnimationIndex).spheres.at(i).endFrame;

		if (currentKeyFrame < startFrame || currentKeyFrame > endFrame) continue;

		DirectX::XMFLOAT4 color = model->animationClips.at(currentAnimationIndex).spheres.at(i).color;
		
		DirectX::XMFLOAT3 playerCollisionPosition = { 0,0,0 };
		float playerRadius = model->animationClips.at(currentAnimationIndex).spheres.at(i).radius;

		if (model->animationClips.at(currentAnimationIndex).spheres.at(i).bindBoneName != "")
		{
			// �{�[�����Ƃ̏���
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

				playerCollisionPosition = { w._41, w._42, w._43 };
				break;
			}
		}
		else
		{
			// �T�C�Y�̏C��
			const float scaleFactor = model->scaleFactors[model->fbxUnit];
			DirectX::XMMATRIX C = DirectX::XMLoadFloat4x4(&model->coordinateSystemTransform[model->coordinateSystemIndex]) * DirectX::XMMatrixScaling(scaleFactor, scaleFactor, scaleFactor);

			DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
			DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(XMConvertToRadians(angle.x), XMConvertToRadians(angle.y), XMConvertToRadians(angle.z));
			DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

			DirectX::XMFLOAT4X4 world;

			playerCollisionPosition = model->animationClips.at(currentAnimationIndex).spheres.at(i).position;

			// ��]��̃R���W�����ʒu���v�Z
			DirectX::XMVECTOR collisionPosVec = DirectX::XMLoadFloat3(&playerCollisionPosition);
			collisionPosVec = DirectX::XMVector3TransformCoord(collisionPosVec, R); // ��]��K�p
			DirectX::XMStoreFloat3(&playerCollisionPosition, collisionPosVec);

			T = DirectX::XMMatrixTranslation(
				position.x + playerCollisionPosition.x,
				position.y + playerCollisionPosition.y,
				position.z + playerCollisionPosition.z
			);

			DirectX::XMStoreFloat4x4(&world, C * S * R * T);

			playerCollisionPosition = { world._41, world._42, world._43 };
		}

		for (int j = 0; j < enemyCount; j++)
		{
			Enemy* enemy = enemyManager.GetEnemy(j);

			// skeletonSphere �����ꍇ
			if (enemy->model->skeletonSpheres.size() > 0)
			{
				for (auto skeletonSphere : enemy->model->skeletonSpheres)
				{

					DirectX::XMFLOAT3 collisionPosition = { 0,0,0 };

					if (skeletonSphere.name != "")
					{
						// �{�[�����Ƃ̏���
						Animation::KeyFrame& keyframe = enemy->model->animationClips.at(enemy->currentAnimationIndex).sequence.at(enemy->currentKeyFrame);

						int nodeCount = keyframe.nodes.size();
						for (int nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
						{
							Animation::KeyFrame::Node& node = keyframe.nodes.at(nodeIndex);

							// �{�[���̃m�[�h�Ɠ������O�Ȃ珈������
							if (node.name != skeletonSphere.name) continue;

							DirectX::XMMATRIX M = DirectX::XMLoadFloat4x4(&node.globalTransform);

							DirectX::XMMATRIX T = DirectX::XMLoadFloat4x4(&enemy->GetTransform());

							DirectX::XMMATRIX W = M * T;
							DirectX::XMFLOAT4X4 w;
							DirectX::XMStoreFloat4x4(&w, W);

							collisionPosition = { w._41, w._42, w._43 };
							break;
						}
					}

					// �Փˏ���
					DirectX::XMFLOAT3 outPosition;
					if (Collision::IntersectSphereVsSphere(
						playerCollisionPosition,
						playerRadius,
						collisionPosition,
						skeletonSphere.radius,
						outPosition)
						)
					{
						// �_���[�W��^����
						//if (enemy->ApplyDamage(1, 0.5f))
						{
							// �q�b�g�G�t�F�N�g�Đ�
							DirectX::XMFLOAT3 e = collisionPosition;
							Effect::Instance().Play(EffectType::Hit1, e, { 0,0,0 });

							// �_���[�W�\��
							int damage = 1;
							std::wstring damageString = std::to_wstring(damage);
							const TCHAR* damageTChar = damageString.c_str();
							DamageText* damageText = new_ DamageText(e, damageTChar);
							DamageTextManager::Instance().Register(damageText);
						}
					}
				}
			}
			// skeletonSphere �������Ȃ��ꍇ
			else
			{
				// �Փˏ���
				DirectX::XMFLOAT3 outPosition;
				if (Collision::IntersectSphereVsCylinder(
					playerCollisionPosition,
					playerRadius,
					enemy->GetPosition(),
					enemy->GetRadius(),
					enemy->GetHeight(),
					outPosition
				))
				{
					// �_���[�W��^����
					if (enemy->ApplyDamage(1, 0.5f))
					{
						// ������΂�
						DirectX::XMFLOAT3 impulse;
						const float power = 10.0f;
						DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
						DirectX::XMVECTOR E = DirectX::XMLoadFloat3(&enemy->GetPosition());
						DirectX::XMVECTOR V = DirectX::XMVectorSubtract(E, P);
						DirectX::XMVECTOR N = DirectX::XMVector3Normalize(V);
						DirectX::XMStoreFloat3(&impulse, N);
						impulse.x *= power;
						//impulse.y *= power;
						impulse.y = power * 0.5f;
						impulse.z *= power;

						enemy->AddImpulse(impulse);

						// �q�b�g�G�t�F�N�g�Đ�
						DirectX::XMFLOAT3 e = enemy->GetPosition();
						e.y += enemy->GetHeight() * 0.5f;
						Effect::Instance().Play(EffectType::Hit1, e, { 0,0,0 });

						// �_���[�W�\��
						int damage = 1;
						std::wstring damageString = std::to_wstring(damage);
						const TCHAR* damageTChar = damageString.c_str();
						DamageText* damageText = new_ DamageText(e, damageTChar);
						DamageTextManager::Instance().Register(damageText);
					}
				}
			}
		}
		
	}
}



//==========================================================================
//
//�@�X�e�[�g����
// 
//==========================================================================

// �ҋ@�X�e�[�g�֑J��
void Player::TransitionIdleState()
{
	// �X�e�[�g�ύX
	state = State::Idle;

	// �u�����h�ݒ�
	blendAnimationIndex = currentAnimationIndex;
	// �A�j���[�V�����ύX
	PlayAnimation(static_cast<int>(PlayerAnim::IdleBattle), true);
}

// �ҋ@�X�e�[�g�X�V����
void Player::UpdateIdleState()
{
	// ------------------------ �ړ����͏��� ------------------------
	if (InputMove())
	{
		TransitionMoveState();
	}
	// �W�����v���͏���
	if (InputJump())
	{
		TransitionJumpState();
	}
	// �U�����͏���
	if (InputAttack())
	{
		TransitionAttackState();
	}
	// �������͏���
	if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::V))
	{
		TransitionVictoryState();
	}
	
	// �e�ۓ��͏���
	InputProjectile();
}

// �ړ��X�e�[�g�֑J��
void Player::TransitionMoveState()
{
	// �X�e�[�g�ύX
	state = State::Move;

	// �u�����h�ݒ�
	blendAnimationIndex = currentAnimationIndex;
	// �A�j���[�V�����ύX
	PlayAnimation(static_cast<int>(PlayerAnim::MoveFwdBattle), true);
}

// �ړ��X�e�[�g�X�V����
void Player::UpdateMoveState()
{
	// ------------------------ �ړ����͏��� ------------------------
	if (!InputMove())
	{
		TransitionIdleState();
	}
	// �W�����v���͏���
	if (InputJump())
	{
		TransitionJumpState();
	}
	// �U�����͏���
	if (InputAttack())
	{
		TransitionAttackState();
	}
	// �e�ۓ��͏���
	InputProjectile();
}

// �W�����v�X�e�[�g�֑J��
void Player::TransitionJumpState()
{
	// �X�e�[�g�ύX
	state = State::Jump;

	// �A�j���[�V�����ύX
	PlayAnimation(static_cast<int>(PlayerAnim::JumpAirNormal), true);
}

// �W�����v�X�e�[�g�X�V����
void Player::UpdateJumpState()
{
	// ------------------------ �ړ����͏��� ------------------------
	InputMove();
	// �W�����v���͏���
	InputJump();

	// �e�ۓ��͏���
	InputProjectile();
}

// �U���X�e�[�g�֑J��
void Player::TransitionAttackState()
{
	// �X�e�[�g�ύX
	state = State::Attack;

	// �A�j���[�V�����ύX
	PlayAnimation(static_cast<int>(PlayerAnim::Attack01), false);
}

// �U���X�e�[�g�X�V����
void Player::UpdateAttackState()
{
	if (animationEndFlag)
	{
		// idle�X�e�[�g�֑J��
		TransitionIdleState();
	}
}

// �����X�e�[�g�֑J��
void Player::TransitionVictoryState()
{
	// �X�e�[�g�ύX
	state = State::Victory;

	// �A�j���[�V�����ύX
	PlayAnimation(static_cast<int>(PlayerAnim::Victory), false);
}

// �����X�e�[�g�X�V����
void Player::UpdateVictoryUpdate()
{
	if (animationEndFlag)
	{
		// idle�X�e�[�g�֑J��
		TransitionIdleState();
	}
}


//==========================================================================
//
//�@�`��
// 
//==========================================================================

// �`�揈��
void Player::Render()
{
	using namespace DirectX;

	// ------------------- Graphics �擾 -------------------
	Graphics& gfx = Graphics::Instance();

	// rasterizer �̐ݒ�
	gfx.SetRasterizer(RASTERIZER_STATE::CLOCK_TRUE_SOLID);

	// --- ���f���`�� ---
	model->Render(transform, { 1,1,1,1 }, &keyFrame);

	//DebugPrimitive::Instance().AddSphere(position, radius, { 1,0,0,1 });
	DebugPrimitive::Instance().AddCylinder(position, radius, height, { 1,0,0,1 });

	// �e�ە`�揈��
	projectileManager.Render();

	// �f�o�b�O�v���~�e�B�u�`��
	DrawDebugPrimitive();
}

// �f�o�b�O�pGUI�`��
void Player::DrawDebugGui()
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
	{
		// �g�����X�t�H�[��
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// �ʒu
			ImGui::DragFloat3("Position", &position.x, 0.1f);
			// ��]
			ImGui::DragFloat3("Angle", &angle.x);
			// �X�P�[��
			ImGui::DragFloat3("Scale", &scale.x, 0.1f);
			// �X�P�[��
			ImGui::DragFloat3("Velocity", &velocity.x, 0.1f);
		}
		ImGui::InputInt("Clip", &currentAnimationIndex);
	}
	ImGui::End();

}