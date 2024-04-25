#include "StEnemy.h""
#include "Library/Graphics/Graphics.h"
#include "Library/MemoryLeak.h"
#include "Library/Timer.h"
#include "Library/3D/DebugPrimitive.h"
#include "Library/3D/ResourceManager.h"
#include "Library/3D/Camera.h"
#include "Library/Input/InputManager.h"
#include "StageManager.h"
#include "ObstacleManager.h"

StEnemy::StEnemy(int enemyKind)
{	
	this->enemyKind = enemyKind;

	model = ResourceManager::Instance().LoadModelResource("Data/Fbx/StEnemy01/Main/StEnemy01Main.fbx");

	paryEffect = ResourceManager::Instance().LoadModelResource("Data/Fbx/paryEffectTest/paryEffectTest.fbx");

	// 敵データの設定
	EnemyData data = enemyData[enemyKind];
	behaviorType = data.behaviorType;
	radius = data.radius;
	pursuitRadius = data.pursuitRadius;
	searchRadius = data.searchRadius;
	notSearchRadius = data.notSearchRadius;
	

	// aiTree の構築
	CreateAiTree();
}

StEnemy::~StEnemy()
{
}

void StEnemy::Update()
{
	// 回転
	DirectX::XMFLOAT3 ang = GetAngle();
	ang.y += rotationSpeed * Timer::Instance().DeltaTime();
	//ang.x = 10;
	SetAngle(ang);

	UpdateTargetPosition();

	aiTree->Update();

	// 速力更新処理
	UpdateVelocity();
	// 無敵時間更新
	UpdateInvincibleTimer();

	//UpdateAnimation();

	// オブジェクト行列更新
	UpdateTransform();
}

void StEnemy::Render(bool drawShadow)
{
	Graphics& gfx = Graphics::Instance();

	gfx.SetRasterizer(static_cast<RASTERIZER_STATE>(RASTERIZER_STATE::CLOCK_FALSE_SOLID));
	model->Render(transform, { 1,1,1,1 }, &keyFrame);

	gfx.SetBlend(BLEND_STATE::ADD);
	gfx.SetRasterizer(static_cast<RASTERIZER_STATE>(RASTERIZER_STATE::CLOCK_FALSE_CULL_NONE));
	if (!drawShadow)
		paryEffect->Render(transform, { 1,1,1,1 }, &keyFrame);

	gfx.SetBlend(BLEND_STATE::ALPHA	);
	gfx.SetRasterizer(static_cast<RASTERIZER_STATE>(RASTERIZER_STATE::CLOCK_FALSE_SOLID));
}

// デバッグプリミティブ描画
void StEnemy::DrawDebugPrimitive()
{
	//DebugPrimitive::Instance().AddSphere(plPosition, 0.5f, { 1,1,0,1 });		// スポーン地点
	//
	//
	////DebugPrimitive::Instance().AddCylinder(position, radius, height, { 1,0,0,1 });
	//
	//DebugPrimitive::Instance().AddSphere(targetPosition, 0.2f, { 0,1,0,1 });	// ターゲット座標
	//DebugPrimitive::Instance().AddSphere(spawnPosition, 0.2f, { 0,1,1,1 });		// スポーン地点
	//
	//DebugPrimitive::Instance().AddCylinder(position, pursuitRadius, 0.2, { 0,1,0,1 });
	//DebugPrimitive::Instance().AddCylinder(position, searchRadius, 0.2, { 1,0,0,1 });
	//DebugPrimitive::Instance().AddCylinder(position, notSearchRadius, 0.2, { 0,0,1,1 });
}

// TargetPosition 更新
void StEnemy::UpdateTargetPosition()
{
	// --- Graphics 取得 ---
	Graphics& gfx = Graphics::Instance();

	// ビューポート
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	gfx.deviceContext->RSGetViewports(&numViewports, &viewport);

	// 変換行列
	DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(&Camera::Instance().GetView());
	DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(&Camera::Instance().GetProjection());
	DirectX::XMMATRIX World = DirectX::XMMatrixIdentity();
	// エネミー配置処理
	InputManager& input = InputManager::Instance();
	if (input.GetMousePress(MOUSEBUTTON_STATE::leftButton))
	{
		// マウスカーソル座標を取得
		DirectX::XMFLOAT3 screenPosition;
		screenPosition.x = static_cast<int>(input.GetCursorPosX());
		screenPosition.y = static_cast<int>(input.GetCursorPosY());

		DirectX::XMVECTOR ScreenPosition, WorldPosition;

		// レイの始点を算出
		screenPosition.z = 0.0f;
		ScreenPosition = DirectX::XMLoadFloat3(&screenPosition);

		WorldPosition = DirectX::XMVector3Unproject(
			ScreenPosition,
			viewport.TopLeftX,
			viewport.TopLeftY,
			viewport.Width,
			viewport.Height,
			viewport.MinDepth,
			viewport.MaxDepth,
			Projection,
			View,
			World
		);
		DirectX::XMFLOAT3 rayStart;
		DirectX::XMStoreFloat3(&rayStart, WorldPosition);

		// レイの終点を算出
		screenPosition.z = 1.0f;
		ScreenPosition = DirectX::XMLoadFloat3(&screenPosition);

		WorldPosition = DirectX::XMVector3Unproject(
			ScreenPosition,
			viewport.TopLeftX,
			viewport.TopLeftY,
			viewport.Width,
			viewport.Height,
			viewport.MinDepth,
			viewport.MaxDepth,
			Projection,
			View,
			World
		);
		DirectX::XMFLOAT3 rayEnd;
		DirectX::XMStoreFloat3(&rayEnd, WorldPosition);

		// レイキャスト
		HitResult hit;
		if (StageManager::Instance().RayCast(rayStart, rayEnd, hit))
		{
			// 敵を配置
			//targetPosition = hit.position;
			plPosition = hit.position;
		}
	}
}

void StEnemy::CreateAiTree()
{
	switch (behaviorType)
	{
	case pursuit:
	{
		// BehaviorTreeを構築
		aiTree = std::make_unique <BTree>(this);

		aiTree->AddNode((int)KIND::NONE, (int)KIND::ROOT, 0, IBTree::RULE::Priority, this);
		aiTree->AddNode((int)KIND::ROOT, (int)KIND::Generate, 0, IBTree::RULE::Non, this);
		aiTree->AddNode((int)KIND::ROOT, (int)KIND::Down, 1, IBTree::RULE::Non, this);
		aiTree->AddNode((int)KIND::ROOT, (int)KIND::Normal, 2, IBTree::RULE::Priority, this);
		aiTree->AddNode((int)KIND::Normal, (int)KIND::PlayerPursuit, 0, IBTree::RULE::Sequence, this);
		aiTree->AddNode((int)KIND::PlayerPursuit, (int)KIND::PlayerPositionGet, 0, IBTree::RULE::Non, this);
		aiTree->AddNode((int)KIND::PlayerPursuit, (int)KIND::WaitChargeAttack, 1, IBTree::RULE::Non, this);
		aiTree->AddNode((int)KIND::PlayerPursuit, (int)KIND::ChargeAttack, 2, IBTree::RULE::Non, this);

		aiTree->AddNode((int)KIND::Normal, (int)KIND::SeekPlayer, 1, IBTree::RULE::Non, this);
		aiTree->AddNode((int)KIND::Normal, (int)KIND::WanderSpawnArea, 2, IBTree::RULE::Non, this);
		break;
	}
	case chase:
	{
		// BehaviorTreeを構築
		aiTree = std::make_unique <BTree>(this);

		aiTree->AddNode((int)KIND::NONE, (int)KIND::ROOT, 0, IBTree::RULE::Priority, this);
		aiTree->AddNode((int)KIND::ROOT, (int)KIND::Generate, 0, IBTree::RULE::Non, this);
		aiTree->AddNode((int)KIND::ROOT, (int)KIND::Down, 1, IBTree::RULE::Non, this);
		aiTree->AddNode((int)KIND::ROOT, (int)KIND::Normal, 2, IBTree::RULE::Priority, this);

		aiTree->AddNode((int)KIND::Normal, (int)KIND::SeekPlayer, 1, IBTree::RULE::Non, this);
		aiTree->AddNode((int)KIND::Normal, (int)KIND::WanderSpawnArea, 2, IBTree::RULE::Non, this);
		break;
	}
	}
}

// 死亡処理
void StEnemy::OnDead()
{
	Obstacle* top;
	Obstacle* middle;
	Obstacle* bottom;

	top = new Obstacle("Data/Fbx/StEnemy01/Top/StEnemy01Top.fbx", false);
	top->SetPosition(GetPosition());
	top->SetRadius(0.0f);
	top->velocity = { 40,0,0 };
	top->SetRotationSpeed(60);
	top->SetFriction(3);
	ObstacleManager::Instance().Register(top);

	middle = new Obstacle("Data/Fbx/StEnemy01/Middle/StEnemy01Middle.fbx", false);
	middle->SetPosition(GetPosition());
	middle->SetRadius(0.0f);
	middle->velocity = { -5,0,10 };
	middle->SetRotationSpeed(30);
	ObstacleManager::Instance().Register(middle);

	bottom = new Obstacle("Data/Fbx/StEnemy01/Bottom/StEnemy01Bottom.fbx", false);
	bottom->SetPosition(GetPosition());
	bottom->SetRadius(0.0f);
	bottom->velocity = { 3,0,-10 };
	bottom->SetRotationSpeed(120);
	bottom->SetAngle({ 90,0,0 });
	ObstacleManager::Instance().Register(bottom);

	Destroy();
}
