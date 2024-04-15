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

	// 角度計算
	CalcOneRadius();

	model = ResourceManager::Instance().LoadModelResource("Data/Fbx/Slime/Slime.fbx");

	// 待機ステートへ遷移
	TransitionIdleState();
}

EnemyContextBaseSlime::~EnemyContextBaseSlime()
{
}

void EnemyContextBaseSlime::Update()
{
	// ステートごとの処理
	switch (state)
	{
	case EnemyContextBaseSlime::State::Idle:
		UpdateIdleState();
		break;
	case EnemyContextBaseSlime::State::Move:
		UpdateMoveState();
		break;
	}

	// 速力更新処理
	UpdateVelocity();
	// 無敵時間更新
	UpdateInvincibleTimer();

	UpdateAnimation();

	// オブジェクト行列更新
	UpdateTransform();
}

void EnemyContextBaseSlime::Render()
{
	using namespace DirectX;

	// --- Graphics 取得 ---
	Graphics& gfx = Graphics::Instance();

	// rasterizer の設定
	gfx.SetRasterizer(RASTERIZER_STATE::CLOCK_TRUE_SOLID);

	model->Render(transform, { 1,1,1,1 }, &keyFrame);

	DebugPrimitive::Instance().AddCylinder(position, radius, height, { 1,0,0,1 });

	// ContextBase 描画
	DrawContext();

	// デバッグ用GUI描画
	DrawDebugGui();
}

// デバッグ用GUI描画
void EnemyContextBaseSlime::DrawDebugGui()
{
	ImGui::Begin("ContextBaseSlime");
	{
		if (ImGui::Button("tuiseki")) TransitionMoveState();
		ImGui::SameLine();
		if (ImGui::Button("Stop")) TransitionIdleState();

		// ウィンドウ内で描画を開始
		ImGui::BeginChild("Direction", ImVec2(300, 300), false);

		ImVec2 p = ImGui::GetCursorScreenPos();
		ImVec2 size = { ImGui::GetWindowSize().x - 1, ImGui::GetWindowSize().y - 1 };
		ImGui::GetWindowDrawList()->AddLine(p, ImVec2(p.x + size.x, p.y), IM_COL32(255, 0, 0, 255));
		ImGui::GetWindowDrawList()->AddLine(p, ImVec2(p.x, p.y + size.y), IM_COL32(255, 0, 0, 255));
		ImGui::GetWindowDrawList()->AddLine(ImVec2(p.x + size.x, p.y), ImVec2(p.x + size.x, p.y + size.y), IM_COL32(255, 0, 0, 255));
		ImGui::GetWindowDrawList()->AddLine(ImVec2(p.x, p.y + size.y), ImVec2(p.x + size.x, p.y + size.y), IM_COL32(255, 0, 0, 255));


		// 中心からの線を描画
		ImVec2 center = ImVec2(p.x + 150, p.y + 150);
		float ImRadius = 100.0f;

		// 円の描画
		ImGui::GetWindowDrawList()->AddCircle(center, ImRadius, IM_COL32(255, 255, 255, 255), 0, 2.0f);

		// マウスの位置を取得して点を描画
		ImVec2 mousePos = ImGui::GetMousePos();
		ImGui::GetWindowDrawList()->AddCircleFilled(mousePos, 5.0f, IM_COL32(255, 0, 0, 255));

		float maxLength = 0.0f;
		for (int index = 0; index < numRays; index++)
		{
			if (maxLength < interest[index]) maxLength = interest[index];
		}

		// interest - danger 描画
		for (int index = 0; index < numRays; index++)
		{
			float radian = DirectX::XMConvertToRadians(rayDirections[index]);
			
			float dot = interest[index] - danger[index];
			dot /= maxLength;
			if (dot <= 0) continue;

			// ImGui 描画に使用するため sinf に - を乗算していない
			ImVec2 end = ImVec2(center.x + cosf(radian) * ImRadius * dot, center.y + sinf(radian) * ImRadius * dot);

			ImGui::GetWindowDrawList()->AddLine(center, end, IM_COL32(0, 255, 0, 255));
		}

		// 最終的な移動方向を描画
		ImVec2 end = ImVec2(center.x + contextVelocity.x * ImRadius, center.y - contextVelocity.z * ImRadius);
		ImGui::GetWindowDrawList()->AddLine(center, end, IM_COL32(255, 255, 0, 255));

		// ウィンドウ内での描画を終了
		ImGui::EndChild();


	}
	ImGui::End();
}

// ContextBase 描画
void EnemyContextBaseSlime::DrawContext()
{
	const DirectX::XMFLOAT4 white = DirectX::XMFLOAT4(0.5f, 0.5f, 0.5f, 1);
	const DirectX::XMFLOAT4 green = DirectX::XMFLOAT4(0, 1, 0, 1);
	const DirectX::XMFLOAT4 yellow = DirectX::XMFLOAT4(1, 1, 0, 1);

	LineRenderer& lineRenderer = LineRenderer::Instance();

	DirectX::XMVECTOR V, P;
	DirectX::XMFLOAT3 basePosition;
	DirectX::XMFLOAT3 position;

	// ベースの白色ライン描画
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

	// interest - danger 描画
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

	// 最終的な移動方向 描画
	basePosition = GetPosition();
	basePosition.y += GetHeight() * 0.32f;
	position.x = basePosition.x + contextVelocity.x * lookAhead;
	position.y = basePosition.y;
	position.z = basePosition.z + contextVelocity.z * lookAhead;

	lineRenderer.AddVertex(basePosition, yellow);
	lineRenderer.AddVertex(position, yellow);
}



// 死亡処理
void EnemyContextBaseSlime::OnDead()
{
	Destroy();
}


//==========================================================================
//
//　ステート処理
// 
//==========================================================================

// 待機ステートへ遷移
void EnemyContextBaseSlime::TransitionIdleState()
{
	// ステート変更
	state = State::Idle;

	// ブレンド設定
	blendAnimationIndex = currentAnimationIndex;
	// アニメーション変更
	PlayAnimation(static_cast<int>(EnemyContextBaseSlimeAnim::IdleNormal), true);
}

// 待機ステート更新処理
void EnemyContextBaseSlime::UpdateIdleState()
{
	// ターゲットチェック
	RayTarget();

	// 興味チェック
	InterestCheck();

	// 危険度チェック
	AccelerationCheck();

	// 最終的な移動ベクトル算出
	CalcContextVec();
}

// 移動ステートへ遷移
void EnemyContextBaseSlime::TransitionMoveState()
{
	// ステート変更
	state = State::Move;

	// ブレンド設定
	blendAnimationIndex = currentAnimationIndex;
	// アニメーション変更
	PlayAnimation(static_cast<int>(EnemyContextBaseSlimeAnim::WalkFWD), true);
}

// 移動ステート更新処理
void EnemyContextBaseSlime::UpdateMoveState()
{
	// ターゲットチェック
	RayTarget();

	// 興味チェック
	InterestCheck();

	// 危険度チェック
	AccelerationCheck();

	// 最終的な移動ベクトル算出
	CalcContextVec();

	velocity = contextVelocity;

	// 旋回処理
	Turn(velocity.x, velocity.z, 720);

	//EndChaseCheck();
}

//==========================================================================
//
//　ContextBase
// 
//==========================================================================

// プレイヤーへのレイキャスト
void EnemyContextBaseSlime::RayTarget()
{
	Player* player = PlayerManager::Instance().GetPlayer(0);
	DirectX::XMFLOAT3 playerPos = player->GetPosition();
	DirectX::XMFLOAT3 enemyPos = GetPosition();

	// レイの開始位置は足元より少し上
	DirectX::XMFLOAT3 start = { position.x, position.y + stepOffset, position.z };

	// レイの終点位置はターゲットの位置
	DirectX::XMFLOAT3 end = { playerPos.x, position.y + stepOffset, playerPos.z };

	// レイキャストによる地面判定
	HitResult hit;
	if (!StageManager::Instance().RayCast(start, end, hit))
	{
		// レイキャストにステージが当たってなければ、位置を更新
		lastTargetPosition = playerPos;
	}
	lastTargetPosition = playerPos;
}

// 角度計算
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

// 興味チェック
void EnemyContextBaseSlime::InterestCheck()
{
	DirectX::XMFLOAT3 targetPos = lastTargetPosition;
	DirectX::XMFLOAT3 enemyPos = GetPosition();

	

	// interest を求める
	for (int index = 0; index < numRays; index++)
	{
		float radian = DirectX::XMConvertToRadians(rayDirections[index]);

		DirectX::XMVECTOR BaseV = { cosf(radian) * lookAhead, 0, -sinf(radian) * lookAhead };
		DirectX::XMVECTOR BaseVN = DirectX::XMVector3Normalize(BaseV);

		// 敵からプレイヤーに向かうベクトル
		DirectX::XMVECTOR PlayerV = { targetPos.x - enemyPos.x, 0, targetPos.z - enemyPos.z };
		float enemyToPlayerDistance = DirectX::XMVectorGetX(DirectX::XMVector3Length(PlayerV));
		DirectX::XMVECTOR PlayerVN = DirectX::XMVector3Normalize(PlayerV);
		
		DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(BaseVN, PlayerVN);

		float dot = 0.0f;
		DirectX::XMStoreFloat(&dot, Dot);

		


		if (enemyToPlayerDistance < targetMaxDistance)
		{
			enemyToPlayerDistance -= targetMinDistance;	// 1までの大きさでまるめる
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

// 危険チェック
void EnemyContextBaseSlime::AccelerationCheck()
{
	// レイの開始位置は足元より少し上
	DirectX::XMFLOAT3 start = { position.x, position.y + stepOffset, position.z };

	for (int index = 0; index < numRays; index++)
	{
		danger[index] = 0;

		float radian = DirectX::XMConvertToRadians(rayDirections[index]);

		float endX = position.x + cosf(radian) * lookAhead;
		float endZ = position.z + (-sinf(radian)) * lookAhead;
		
		// レイの終点位置は移動後の位置
		DirectX::XMFLOAT3 end = { endX, position.y + stepOffset, endZ };

		// レイキャストによる地面判定
		HitResult hit;
		if (StageManager::Instance().RayCast(start, end, hit))
		{
			// 壁に近づく程 weight の値が大きくなる
			float weight = 1.0f - ((hit.distance - 1.0f) / lookAhead);

			if (danger[index] <= 1 * weight)
			{
				danger[index] = 1 * weight;
			}
			//danger[index] += 1 * weight;
		}

		// レイキャストによる地面判定
		if (EnemyManager::Instance().RayCast(start, end, hit, id))
		{
			// 壁に近づく程 weight の値が大きくなる
			//float weight = 1.0f - (hit.distance / lookAhead);
			//danger[index] += 1 * weight;
			danger[index] = 1;
		}

		if (danger[index] > 1.0f) danger[index] = 1.0f;
	}
}

// 最終的な移動ベクトル算出
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

// 追跡終了チェック
void EnemyContextBaseSlime::EndChaseCheck()
{
	float offset = 0.01f;
	if (!(lastTargetPosition.x > position.x - offset && lastTargetPosition.x <= position.x + offset)) return;
	if (!(lastTargetPosition.y > position.y - offset && lastTargetPosition.y <= position.y + offset)) return;
	if (!(lastTargetPosition.z > position.z - offset && lastTargetPosition.z <= position.z + offset)) return;

	TransitionIdleState();
}

