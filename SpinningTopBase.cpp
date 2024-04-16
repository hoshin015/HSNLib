#include "SpinningTopBase.h"
#include "Library/Timer.h"
#include "Library/ImGui/ConsoleData.h"
#include "Library/3D/DebugPrimitive.h"
#include "StageManager.h"

// 行列更新処理
void SpinningTopBase::UpdateTransform()
{
	// サイズ調整行列を作成
	const float scaleFactor = model->scaleFactors[model->fbxUnit];
	DirectX::XMMATRIX C = DirectX::XMLoadFloat4x4(&model->coordinateSystemTransform[model->coordinateSystemIndex]) * DirectX::XMMatrixScaling(scaleFactor, scaleFactor, scaleFactor);

	// スケール行列を作成
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	// 回転行列を作成
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(angle.x), DirectX::XMConvertToRadians(angle.y), DirectX::XMConvertToRadians(angle.z));
	// 位置行列を作成
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	// ４つの行列を組み合わせ、ワールド行列を作成
	DirectX::XMMATRIX W = C * S * R * T;
	// 計算したワールド行列を取り出す
	DirectX::XMStoreFloat4x4(&transform, W);
}

// ダメージを与える
bool SpinningTopBase::ApplyDamage(int damage, float invincibleTime)
{
	if (damage <= 0) return false;

	if (health <= 0) return false;

	if (invincibleTimer > 0.0f) return false;

	// 無敵時間設定
	invincibleTimer = invincibleTime;

	// ダメージ処理
	health -= damage;

	// 死亡通知
	if (health <= 0)
	{
		OnDead();
	}
	// ダメージ通知
	else
	{
		OnDamaged();
	}

	// 健康状態が変更した場合はtrueを返す
	return true;
}

// 衝撃を与える
void SpinningTopBase::AddImpulse(const DirectX::XMFLOAT3& impulse)
{
	// 速力に力を加える
	velocity.x += impulse.x;
	velocity.y += impulse.y;
	velocity.z += impulse.z;
}

// 移動処理
void SpinningTopBase::Move(float vx, float vz, float speed)
{
	// 移動方向ベクトルを設定
	moveVecX = vx;
	moveVecZ = vz;

	// 最大速度設定
	maxMoveSpeed = speed;
}

// 旋回処理
void SpinningTopBase::Turn(float vx, float vz, float speed)
{
	speed *= Timer::Instance().DeltaTime();

	// 進行ベクトルがゼロベクトルの場合は処理する必要なし
	float length = sqrtf(vx * vx + vz * vz);
	if (length < 0.001f)
	{
		return;
	}

	// 進行ベクトルを単位ベクトル化
	vx /= length;
	vz /= length;

	// 自身の回転値から前方向を求める
	float frontX = sinf(DirectX::XMConvertToRadians(angle.y));
	float frontZ = cosf(DirectX::XMConvertToRadians(angle.y));

	// 回転角を求めるため、２つの単位ベクトルの内積を計算する
	float dot = (frontX * vx) + (frontZ * vz);

	// 内積値は-1.0～1.0で表現されており、２つの単位ベクトルの角度が
	// 小さいほど1.0に近づくという性質を利用して回転速度を調整する
	float rot = 1.0f - dot;
	rot *= 60;
	if (rot > speed) rot = speed;


	// 左右判定を行うために２つの単位ベクトルの外積を計算する
	float cross = (frontX * vz) - (frontZ * vx);

	// 2D外積値が正の場合か負の場合によって左右判定が行える
	// 左右判定を行うことによって左右回転を行う
	if (cross < 0.0f)
	{
		angle.y += rot;
	}
	else
	{
		angle.y -= rot;
	}
}

// ジャンプ処理
void SpinningTopBase::Jump(float speed)
{
	// 上方向の力を設定
	velocity.y = speed;
}

void SpinningTopBase::UpdateVelocity()
{
	// 垂直速力更新処理
	UpdateVerticalVelocity();
	// 垂直移動更新処理
	UpdateVerticalMove();

	// 水平速力更新処理
	UpdateHorizontalVelocity();
	// 水平移動更新処理
	UpdateHorizontalMove();
}

// 無敵時間処理
void SpinningTopBase::UpdateInvincibleTimer()
{
	if (invincibleTimer > 0.0f)
	{
		invincibleTimer -= Timer::Instance().DeltaTime();
	}
}

// 垂直速力更新処理
void SpinningTopBase::UpdateVerticalVelocity()
{
	// 重力処理
	velocity.y += gravity * Timer::Instance().DeltaTime() * 60;
}

// 垂直移動更新処理
void SpinningTopBase::UpdateVerticalMove()
{
	// 垂直方向の移動量
	float my = velocity.y * Timer::Instance().DeltaTime();

	// キャラクターのY軸方向となる法線ベクトル
	DirectX::XMFLOAT3 normal = { 0,1,0 };

	slopeRate = 0.0f;

	// 落下中
	if (my < 0.0f)
	{
		// レイの開始位置は足元より少し上
		DirectX::XMFLOAT3 start = { position.x, position.y + stepOffset, position.z };
		// レイの終点位置は移動後の位置
		DirectX::XMFLOAT3 end = { position.x, position.y + my, position.z };

		// レイキャストによる地面判定
		HitResult hit;
		if (StageManager::Instance().RayCast(start, end, hit))
		{
			// 法線ベクトル取得
			normal = hit.normal;

			// 傾斜率の計算
			float normalLengthXZ = sqrtf(hit.normal.x * hit.normal.x + hit.normal.z * hit.normal.z);
			slopeRate = 1.0f - (hit.normal.y / (normalLengthXZ + hit.normal.y));

			// 地面に接地している
			position.y = hit.position.y;

			// 着地した
			if (!isGround)
			{
				OnLanding();
			}
			isGround = true;
			velocity.y = 0;
		}
		else
		{
			// 空中に浮いている
			position.y += my;
			isGround = false;
		}
	}
	// 上昇中
	else if (my > 0.0f)
	{
		position.y += my;
		isGround = false;
	}
}

// 水平速力更新処理
void SpinningTopBase::UpdateHorizontalVelocity()
{
	// XZ平面の速力を減速する
	float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
	if (length > 0.0f)
	{
		// 摩擦力
		float friction = this->friction * Timer::Instance().DeltaTime() * 60.0f;
		// 空中にいるときは摩擦力を減らす
		if (!isGround) friction *= airControl;

		// 摩擦による横方向の減速処理
		if (length > friction)
		{
			// 単位ベクトル化
			float vx = velocity.x / length;
			float vz = velocity.z / length;
			velocity.x -= vx * friction;
			velocity.z -= vz * friction;

		}
		// 横方向の速力が摩擦力以下になったので速力を無効化
		else
		{
			velocity.x = 0.0f;
			velocity.z = 0.0f;
		}
	}

	// XZ平面の速力を加速する
	if (length <= maxMoveSpeed)
	{
		// 移動ベクトルがゼロベクトルでないなら加速する
		float moveVecLength = sqrtf(moveVecX * moveVecX + moveVecZ * moveVecZ);
		if (moveVecLength > 0.0f)
		{
			// 加速力
			float acceleration = this->acceleration * Timer::Instance().DeltaTime() * 60.0f;
			// 空中にいるときは摩擦力を減らす
			if (!isGround) acceleration *= airControl;
			// 移動ベクトルによる加速処理
			velocity.x += moveVecX * acceleration;
			velocity.z += moveVecZ * acceleration;
			// 最大速度制限
			float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
			if (length > maxMoveSpeed)
			{
				float vx = velocity.x / length;
				float vz = velocity.z / length;
				velocity.x = vx * maxMoveSpeed;
				velocity.z = vz * maxMoveSpeed;
			}

			// 下り坂でガタガタしないようにする
			if (isGround && slopeRate > 0.0f)
			{
				velocity.y -= length * slopeRate * Timer::Instance().DeltaTime() * 60.0f;
			}
		}
	}
	// 最大速度制限
	length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
	if (length > maxMoveSpeed)
	{
		float vx = velocity.x / length;
		float vz = velocity.z / length;
		velocity.x = vx * maxMoveSpeed;
		velocity.z = vz * maxMoveSpeed;
	}

	// 移動ベクトルゼロ
	moveVecX = 0;
	moveVecZ = 0;
}

// 水平移動更新処理
void SpinningTopBase::UpdateHorizontalMove()
{
	// 水平速力量計算
	float velocityLengthXZ = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
	if (velocityLengthXZ > 0.0f)
	{
		// 水平移動値
		float mx = velocity.x * Timer::Instance().DeltaTime();
		float mz = velocity.z * Timer::Instance().DeltaTime();

		// レイの開始位置と終点位置
		DirectX::XMFLOAT3 start = { position.x, position.y + stepOffset, position.z };
		DirectX::XMFLOAT3 end = { position.x + mx , position.y + stepOffset, position.z + mz };

		// レイキャストによる壁判定
		HitResult hit;
		if (StageManager::Instance().RayCast(start, end, hit))
		{
			// 壁までのベクトル
			DirectX::XMVECTOR Start = DirectX::XMLoadFloat3(&start);
			DirectX::XMVECTOR End = DirectX::XMLoadFloat3(&end);
			DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(End, Start);

			// 壁の法線
			DirectX::XMVECTOR Normal = DirectX::XMLoadFloat3(&hit.normal);
			DirectX::XMVectorScale(Normal, 1.1f);

			// 入射ベクトルを法線に射影
			DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(DirectX::XMVectorNegate(Vec), Normal);

			// 補正位置の計算
			DirectX::XMVECTOR CollectPosition = DirectX::XMVectorMultiplyAdd(Normal, Dot, End);
			DirectX::XMFLOAT3 collectPosition;
			DirectX::XMStoreFloat3(&collectPosition, CollectPosition);

			// 壁ずり方向へレイキャスト
			HitResult hit2;
			if (!StageManager::Instance().RayCast(hit.position, collectPosition, hit2))
			{
				// 壁ずり方向で壁にあたらなかったら補正位置に移動
				position.x = collectPosition.x;
				position.z = collectPosition.z;
			}
			else
			{
				// 壁までのベクトル
				DirectX::XMVECTOR Start2 = DirectX::XMLoadFloat3(&hit.position);
				DirectX::XMVECTOR End2 = DirectX::XMLoadFloat3(&collectPosition);
				DirectX::XMVECTOR Vec2 = DirectX::XMVectorSubtract(End2, Start2);

				// 壁の法線
				DirectX::XMVECTOR Normal2 = DirectX::XMLoadFloat3(&hit2.normal);
				DirectX::XMVectorScale(Normal2, 1.1f);

				// 入射ベクトルを法線に射影
				DirectX::XMVECTOR Dot2 = DirectX::XMVector3Dot(DirectX::XMVectorNegate(Vec2), Normal2);

				// 補正位置の計算
				DirectX::XMVECTOR CollectPosition2 = DirectX::XMVectorMultiplyAdd(Normal2, Dot2, End2);
				DirectX::XMFLOAT3 collectPosition2;
				DirectX::XMStoreFloat3(&collectPosition2, CollectPosition2);

				// 壁ずり方向へレイキャスト
				HitResult hit3;
				if (!StageManager::Instance().RayCast(hit2.position, collectPosition2, hit3))
				{
					// 壁ずり方向で壁にあたらなかったら補正位置に移動
					position.x = collectPosition2.x;
					position.z = collectPosition2.z;
				}
				else
				{
					// 壁までのベクトル
					DirectX::XMVECTOR Start3 = DirectX::XMLoadFloat3(&hit2.position);
					DirectX::XMVECTOR End3 = DirectX::XMLoadFloat3(&collectPosition2);
					DirectX::XMVECTOR Vec3 = DirectX::XMVectorSubtract(End3, Start3);

					// 壁の法線
					DirectX::XMVECTOR Normal3 = DirectX::XMLoadFloat3(&hit3.normal);
					DirectX::XMVectorScale(Normal3, 1.1f);

					// 入射ベクトルを法線に射影
					DirectX::XMVECTOR Dot3 = DirectX::XMVector3Dot(DirectX::XMVectorNegate(Vec3), Normal3);

					// 補正位置の計算
					DirectX::XMVECTOR CollectPosition3 = DirectX::XMVectorMultiplyAdd(Normal3, Dot3, End3);
					DirectX::XMFLOAT3 collectPosition3;
					DirectX::XMStoreFloat3(&collectPosition3, CollectPosition3);

					// 壁ずり方向へレイキャスト
					HitResult hit4;
					if (!StageManager::Instance().RayCast(hit3.position, collectPosition3, hit4))
					{
						// 壁ずり方向で壁にあたらなかったら補正位置に移動
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
		// 壁に衝突しなかった場合
		else
		{
			// 移動
			position.x += mx;
			position.z += mz;
		}
	}
}

//==========================================================================
//
//　エフェクト処理
// 
//==========================================================================

// エフェクト更新処理
void SpinningTopBase::UpdateEffect()
{
	using namespace DirectX;

	// サイズの修正
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
		// 既に再生されているなら再生しない
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

		// 回転後のコリジョン位置を計算
		DirectX::XMVECTOR effectPosVec = DirectX::XMLoadFloat3(&effectPosition);
		effectPosVec = DirectX::XMVector3TransformCoord(effectPosVec, R); // 回転を適用
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

// エフェクトフラグクリア
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
//　効果音処理
// 
//==========================================================================

// 効果音更新処理
void SpinningTopBase::UpdateSE()
{
	int seCount = model->animationClips.at(currentAnimationIndex).animSEs.size();

	for (int i = 0; i < seCount; i++)
	{
		// 既に再生されているなら再生しない
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

// 効果音フラグクリア
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
//　アニメーション処理
// 
//==========================================================================

// アニメーション再生
void SpinningTopBase::PlayAnimation(int index, bool loop)
{
	currentAnimationIndex = index;
	currentAnimationSeconds = 0.0f;

	animationLoopFlag = loop;
	animationEndFlag = false;

	animationBlendTime = 0.0f;
	animationBlendSeconds = 0.2f;
}

// アニメーション更新
void SpinningTopBase::UpdateAnimation()
{
	// 最終フレーム処理
	if (animationEndFlag) return;


	if (blendAnimationIndex != -1)
	{
		// ブレンド率の計算
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
	// 現在のフレームが最大フレームを超えていたら
	if (currentAnimationSeconds >= animation.secondsLength)
	{
		if (animationLoopFlag)
		{
			// 先頭秒数へ
			currentKeyFrame = 0;
			currentAnimationSeconds -= animation.secondsLength;
		}
		else
		{
			// 最終秒数で停止
			currentKeyFrame = animation.sequence.size() - 1;
			currentAnimationSeconds = animation.secondsLength;
			animationEndFlag = true;
		}
		// エフェクトフラグのクリア
		ClearEffectFlag();
		// 効果音フラグのクリア
		ClearSEFlag();
	}

	// keyFrame設定
	keyFrame = animation.sequence.at(currentKeyFrame);
}


//==========================================================================
//
//　デバッグプリミティブ描画
// 
//==========================================================================

void SpinningTopBase::DrawDebugPrimitive()
{
	// --- コリジョン描画 ---
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

					// ボーンのノードと同じ名前なら処理する
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
				// サイズの修正
				const float scaleFactor = model->scaleFactors[model->fbxUnit];
				DirectX::XMMATRIX C = DirectX::XMLoadFloat4x4(&model->coordinateSystemTransform[model->coordinateSystemIndex]) * DirectX::XMMatrixScaling(scaleFactor, scaleFactor, scaleFactor);

				DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
				DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(angle.x), DirectX::XMConvertToRadians(angle.y), DirectX::XMConvertToRadians(angle.z));
				DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

				DirectX::XMFLOAT4X4 world;

				collisionPosition = model->animationClips.at(currentAnimationIndex).spheres.at(i).position;

				// 回転後のコリジョン位置を計算
				DirectX::XMVECTOR collisionPosVec = DirectX::XMLoadFloat3(&collisionPosition);
				collisionPosVec = DirectX::XMVector3TransformCoord(collisionPosVec, R); // 回転を適用
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
