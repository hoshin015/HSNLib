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
//　初期化・終了化
// 
//==========================================================================

// コンストラクタ
Player::Player()
{
	//model = ResourceManager::Instance().LoadModelResource("Data/Fbx/Chara/character.fbx");

	model = std::make_unique<SkinnedMesh>("Data/Fbx/Chara/character.fbx");

	// 待機ステートへ遷移
	TransitionIdleState();
}

// デストラクタ
Player::~Player()
{
}


//==========================================================================
//
//　更新処理
// 
//==========================================================================

// 更新処理
void Player::Update()
{
	// ステートごとの処理
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
	
	// 速力更新処理
	UpdateVelocity();

	// ----------------- プレイヤーと敵との衝突処理 -----------------
	CollisionPlayerVsEnemies();
	// 弾丸と敵の衝突処理
	CollisionProjectilesVsEnemies();
	// アニメーションと敵の衝突処理
	CollisionNodeVsEnemies();


	// 弾丸更新処理
	projectileManager.Update();

	UpdateAnimation();

	UpdateEffect();

	UpdateSE();

	// ------------------- オブジェクト行列を更新 -------------------
	UpdateTransform();
}

// 移動入力処理
bool Player::InputMove()
{
	// 進行ベクトル取得
	DirectX::XMFLOAT3 moveVec = GetMoveVec();
	// 移動処理
	Move(moveVec.x, moveVec.z, moveSpeed);
	// 旋回処理
	Turn(moveVec.x, moveVec.z, turnSpeed);

	// 進行ベクトルがゼロベクトルでない場合は入力された
	return moveVec.x != 0 || moveVec.z != 0;
}

DirectX::XMFLOAT3 Player::GetMoveVec() const
{
	// --- 入力処理 ---
	InputManager& inputManager = InputManager::Instance();

	float ax = 0;
	float ay = 0;

	if (inputManager.GetKeyPress(DirectX::Keyboard::A)) ax--;
	if (inputManager.GetKeyPress(DirectX::Keyboard::D)) ax++;
	if (inputManager.GetKeyPress(DirectX::Keyboard::W)) ay++;
	if (inputManager.GetKeyPress(DirectX::Keyboard::S)) ay--;

	// カメラ方向とスティックの入力値によって進行方向を計算する
	Camera& camera = Camera::Instance();
	const DirectX::XMFLOAT3& cameraRight = camera.GetRight();
	const DirectX::XMFLOAT3& cameraFront = camera.GetFront();

	// 移動ベクトルはXZ平面に水平なベクトルになるようにする

	// カメラ右方向ベクトルをXZ単位ベクトルに変換
	float cameraRightX = cameraRight.x;
	float cameraRightZ = cameraRight.z;
	float cameraRightLength = sqrtf(cameraRight.x * cameraRight.x + cameraRight.y * cameraRight.y + cameraRight.z * cameraRight.z);
	if (cameraRightLength > 0.0f)
	{
		// 単位ベクトル化
		cameraRightX /= cameraRightLength;
		cameraRightZ /= cameraRightLength;
	}

	// カメラ前方向ベクトルをXZ単位ベクトルに変換
	float cameraFrontX = cameraFront.x;
	float cameraFrontZ = cameraFront.z;
	float cameraFrontLength = sqrtf(cameraFront.x * cameraFront.x + cameraFront.y * cameraFront.y + cameraFront.z * cameraFront.z);
	if (cameraFrontLength > 0.0f)
	{
		// 単位ベクトル化
		cameraFrontX /= cameraFrontLength;
		cameraFrontZ /= cameraFrontLength;
	}

	// スティックの水平入力値をカメラ右方向に反映し、
	// スティックの垂直入力値をカメラ前方向に反映し、
	// 進行ベクトルを計算する
	DirectX::XMFLOAT3 vec;
	vec.x = ax * cameraRightX + ay * cameraFrontX;
	vec.z = ax * cameraRightZ + ay * cameraFrontZ;
	// Y軸方向には移動しない
	vec.y = 0.0f;

	return vec;
}

bool Player::InputJump()
{
	InputManager& input = InputManager::Instance();
	if (input.GetKeyPressed(Keyboard::Space))
	{
		// ジャンプ回数制限
		if (jumpCount < jumpLimit)
		{
			// ジャンプ
			jumpCount++;
			Jump(jumpSpeed);

			return true;
		}
	}
	return false;
}

// 攻撃入力処理
bool Player::InputAttack()
{
	InputManager& input = InputManager::Instance();
	if (input.GetMousePressed(MOUSEBUTTON_STATE::leftButton))
	{
		return true;
	}

	return false;
}

// 着地処理
void Player::OnLanding()
{
	jumpCount = 0;

	// 着地ステートへ遷移
	TransitionIdleState();
}

// 弾丸入力処理
void Player::InputProjectile()
{
	InputManager& inputManager = InputManager::Instance();

	// 直進弾丸発射
	if (inputManager.GetKeyPressed(Keyboard::Z))
	{
		// 前方向
		DirectX::XMFLOAT3 dir;
		dir.x = sinf(DirectX::XMConvertToRadians(angle.y));
		dir.y = 0.0f;
		dir.z = cosf(DirectX::XMConvertToRadians(angle.y));
		// 発射位置(プレイヤーの腰あたり)
		DirectX::XMFLOAT3 pos;
		pos.x = position.x;
		pos.y = position.y + height * 0.5f;
		pos.z = position.z;
		// 発射
		ProjectileStraight* projectile = new_ ProjectileStraight(&projectileManager);
		projectile->Launch(dir, pos);
	}

	// 追尾弾丸発射
	if (inputManager.GetKeyPressed(Keyboard::X))
	{
		// 前方向
		DirectX::XMFLOAT3 dir;
		dir.x = sinf(DirectX::XMConvertToRadians(angle.y));
		dir.y = 0.0f;
		dir.z = cosf(DirectX::XMConvertToRadians(angle.y));
		// 発射位置(プレイヤーの腰あたり)
		DirectX::XMFLOAT3 pos;
		pos.x = position.x;
		pos.y = position.y + height * 0.5f;
		pos.z = position.z;
		// ターゲット（デフォルトではプレイヤーの前方）
		DirectX::XMFLOAT3 target;
		target.x = pos.x + dir.x * 1000.0f;
		target.y = pos.y + dir.y * 1000.0f;
		target.z = pos.z + dir.z * 1000.0f;

		// 一番近くの敵をターゲットにする
		float dist = FLT_MAX;
		EnemyManager& enemyManager = EnemyManager::Instance();
		int enemyCount = enemyManager.GetEnemyCount();
		for (int i = 0; i < enemyCount; i++)
		{
			// 敵との距離判定
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

		// 発射
		ProjectileHoming* projectile = new_ ProjectileHoming(&projectileManager);
		projectile->Launch(dir, pos, target);
	}
}

// プレイヤーとエネミーのくらい判定の衝突
void Player::CollisionBodyPlayerVsEnemies()
{

}

// プレイヤーとエネミーの衝突処理
void Player::CollisionPlayerVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	enemyManager.SortPlaySortEnemiesByDistanceToPlayerEnemy(GetPosition());

	// 全ての敵と総当たりで衝突判定
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
					// ボーンごとの処理
					Animation::KeyFrame& keyframe = enemy->model->animationClips.at(enemy->currentAnimationIndex).sequence.at(enemy->currentKeyFrame);

					int nodeCount = keyframe.nodes.size();
					for (int nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
					{
						Animation::KeyFrame::Node& node = keyframe.nodes.at(nodeIndex);

						// ボーンのノードと同じ名前なら処理する
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

				// 衝突処理
				DirectX::XMFLOAT3 outPosition;
				if (Collision::IntersectSphereVsSphere(
					collisionPosition,
					skeletonRadius,
					position,
					radius,
					outPosition)
					)
				{
					// 押し出し後の位置設定
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
					// 押し出し後の位置設定
					SetPosition(outPosition);
				}
			}
		}		
	}
}

// 弾丸と敵の衝突処理
void Player::CollisionProjectilesVsEnemies()
{
	EnemyManager& enemyManager = EnemyManager::Instance();

	// 全ての弾丸と全ての敵を総当たりで衝突処理
	int projectileCount = projectileManager.GetProjectileCount();
	int enemyCount = enemyManager.GetEnemyCount();
	for (int i = 0; i < projectileCount; i++)
	{
		Projectile* projectile = projectileManager.GetProjectile(i);

		for (int j = 0; j < enemyCount; j++)
		{
			Enemy* enemy = enemyManager.GetEnemy(j);

			// 衝突処理
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
				// ダメージを与える
				if (enemy->ApplyDamage(1, 0.5f))
				{
					// 吹き飛ばす
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

					// ヒットエフェクト再生
					DirectX::XMFLOAT3 e = enemy->GetPosition();
					e.y += enemy->GetHeight() * 0.5f;
					Effect::Instance().Play(EffectType::Hit1, e, {0,0,0});

					// 弾丸破棄
					projectile->Destroy();

					// ダメージ表示
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

// アニメーションとエネミーの衝突処理
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
			// ボーンごとの処理
			Animation::KeyFrame& keyframe = model->animationClips.at(currentAnimationIndex).sequence.at(currentKeyFrame);

			int nodeCount = keyframe.nodes.size();
			for (int nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
			{
				Animation::KeyFrame::Node& node = keyframe.nodes.at(nodeIndex);

				// ボーンのノードと同じ名前なら処理する
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
			// サイズの修正
			const float scaleFactor = model->scaleFactors[model->fbxUnit];
			DirectX::XMMATRIX C = DirectX::XMLoadFloat4x4(&model->coordinateSystemTransform[model->coordinateSystemIndex]) * DirectX::XMMatrixScaling(scaleFactor, scaleFactor, scaleFactor);

			DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
			DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(XMConvertToRadians(angle.x), XMConvertToRadians(angle.y), XMConvertToRadians(angle.z));
			DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

			DirectX::XMFLOAT4X4 world;

			playerCollisionPosition = model->animationClips.at(currentAnimationIndex).spheres.at(i).position;

			// 回転後のコリジョン位置を計算
			DirectX::XMVECTOR collisionPosVec = DirectX::XMLoadFloat3(&playerCollisionPosition);
			collisionPosVec = DirectX::XMVector3TransformCoord(collisionPosVec, R); // 回転を適用
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

			// skeletonSphere を持つ場合
			if (enemy->model->skeletonSpheres.size() > 0)
			{
				for (auto skeletonSphere : enemy->model->skeletonSpheres)
				{

					DirectX::XMFLOAT3 collisionPosition = { 0,0,0 };

					if (skeletonSphere.name != "")
					{
						// ボーンごとの処理
						Animation::KeyFrame& keyframe = enemy->model->animationClips.at(enemy->currentAnimationIndex).sequence.at(enemy->currentKeyFrame);

						int nodeCount = keyframe.nodes.size();
						for (int nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
						{
							Animation::KeyFrame::Node& node = keyframe.nodes.at(nodeIndex);

							// ボーンのノードと同じ名前なら処理する
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

					// 衝突処理
					DirectX::XMFLOAT3 outPosition;
					if (Collision::IntersectSphereVsSphere(
						playerCollisionPosition,
						playerRadius,
						collisionPosition,
						skeletonSphere.radius,
						outPosition)
						)
					{
						// ダメージを与える
						//if (enemy->ApplyDamage(1, 0.5f))
						{
							// ヒットエフェクト再生
							DirectX::XMFLOAT3 e = collisionPosition;
							Effect::Instance().Play(EffectType::Hit1, e, { 0,0,0 });

							// ダメージ表示
							int damage = 1;
							std::wstring damageString = std::to_wstring(damage);
							const TCHAR* damageTChar = damageString.c_str();
							DamageText* damageText = new_ DamageText(e, damageTChar);
							DamageTextManager::Instance().Register(damageText);
						}
					}
				}
			}
			// skeletonSphere を持たない場合
			else
			{
				// 衝突処理
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
					// ダメージを与える
					if (enemy->ApplyDamage(1, 0.5f))
					{
						// 吹き飛ばす
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

						// ヒットエフェクト再生
						DirectX::XMFLOAT3 e = enemy->GetPosition();
						e.y += enemy->GetHeight() * 0.5f;
						Effect::Instance().Play(EffectType::Hit1, e, { 0,0,0 });

						// ダメージ表示
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
//　ステート処理
// 
//==========================================================================

// 待機ステートへ遷移
void Player::TransitionIdleState()
{
	// ステート変更
	state = State::Idle;

	// ブレンド設定
	blendAnimationIndex = currentAnimationIndex;
	// アニメーション変更
	PlayAnimation(static_cast<int>(PlayerAnim::IdleBattle), true);
}

// 待機ステート更新処理
void Player::UpdateIdleState()
{
	// ------------------------ 移動入力処理 ------------------------
	if (InputMove())
	{
		TransitionMoveState();
	}
	// ジャンプ入力処理
	if (InputJump())
	{
		TransitionJumpState();
	}
	// 攻撃入力処理
	if (InputAttack())
	{
		TransitionAttackState();
	}
	// 勝利入力処理
	if (InputManager::Instance().GetKeyPressed(DirectX::Keyboard::V))
	{
		TransitionVictoryState();
	}
	
	// 弾丸入力処理
	InputProjectile();
}

// 移動ステートへ遷移
void Player::TransitionMoveState()
{
	// ステート変更
	state = State::Move;

	// ブレンド設定
	blendAnimationIndex = currentAnimationIndex;
	// アニメーション変更
	PlayAnimation(static_cast<int>(PlayerAnim::MoveFwdBattle), true);
}

// 移動ステート更新処理
void Player::UpdateMoveState()
{
	// ------------------------ 移動入力処理 ------------------------
	if (!InputMove())
	{
		TransitionIdleState();
	}
	// ジャンプ入力処理
	if (InputJump())
	{
		TransitionJumpState();
	}
	// 攻撃入力処理
	if (InputAttack())
	{
		TransitionAttackState();
	}
	// 弾丸入力処理
	InputProjectile();
}

// ジャンプステートへ遷移
void Player::TransitionJumpState()
{
	// ステート変更
	state = State::Jump;

	// アニメーション変更
	PlayAnimation(static_cast<int>(PlayerAnim::JumpAirNormal), true);
}

// ジャンプステート更新処理
void Player::UpdateJumpState()
{
	// ------------------------ 移動入力処理 ------------------------
	InputMove();
	// ジャンプ入力処理
	InputJump();

	// 弾丸入力処理
	InputProjectile();
}

// 攻撃ステートへ遷移
void Player::TransitionAttackState()
{
	// ステート変更
	state = State::Attack;

	// アニメーション変更
	PlayAnimation(static_cast<int>(PlayerAnim::Attack01), false);
}

// 攻撃ステート更新処理
void Player::UpdateAttackState()
{
	if (animationEndFlag)
	{
		// idleステートへ遷移
		TransitionIdleState();
	}
}

// 勝利ステートへ遷移
void Player::TransitionVictoryState()
{
	// ステート変更
	state = State::Victory;

	// アニメーション変更
	PlayAnimation(static_cast<int>(PlayerAnim::Victory), false);
}

// 勝利ステート更新処理
void Player::UpdateVictoryUpdate()
{
	if (animationEndFlag)
	{
		// idleステートへ遷移
		TransitionIdleState();
	}
}


//==========================================================================
//
//　描画
// 
//==========================================================================

// 描画処理
void Player::Render()
{
	using namespace DirectX;

	// ------------------- Graphics 取得 -------------------
	Graphics& gfx = Graphics::Instance();

	// rasterizer の設定
	gfx.SetRasterizer(RASTERIZER_STATE::CLOCK_TRUE_SOLID);

	// --- モデル描画 ---
	model->Render(transform, { 1,1,1,1 }, &keyFrame);

	//DebugPrimitive::Instance().AddSphere(position, radius, { 1,0,0,1 });
	DebugPrimitive::Instance().AddCylinder(position, radius, height, { 1,0,0,1 });

	// 弾丸描画処理
	projectileManager.Render();

	// デバッグプリミティブ描画
	DrawDebugPrimitive();
}

// デバッグ用GUI描画
void Player::DrawDebugGui()
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Player", nullptr, ImGuiWindowFlags_None))
	{
		// トランスフォーム
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// 位置
			ImGui::DragFloat3("Position", &position.x, 0.1f);
			// 回転
			ImGui::DragFloat3("Angle", &angle.x);
			// スケール
			ImGui::DragFloat3("Scale", &scale.x, 0.1f);
			// スケール
			ImGui::DragFloat3("Velocity", &velocity.x, 0.1f);
		}
		ImGui::InputInt("Clip", &currentAnimationIndex);
	}
	ImGui::End();

}