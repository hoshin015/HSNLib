#include "ProjectileHoming.h"
#include "Library/Timer.h"
#include "Library/MemoryLeak.h"

// コンストラクタ
ProjectileHoming::ProjectileHoming(ProjectileManager* manager) : Projectile(manager)
{
	model = new_ SkinnedMesh("Data/Fbx/Sword/Sword.fbx", true);

	scale.x = scale.y = scale.z = 3.0f;
}

// デストラクタ
ProjectileHoming::~ProjectileHoming()
{
	delete model;
}

// 更新処理
void ProjectileHoming::Update()
{
	// 寿命処理
	lifeTimer -= Timer::Instance().DeltaTime();
	if (lifeTimer <= 0)
	{
		Destroy();
	}

	// 移動
	float moveSpeed = this->moveSpeed * Timer::Instance().DeltaTime();
	position.x += direction.x * moveSpeed;
	position.y += direction.y * moveSpeed;
	position.z += direction.z * moveSpeed;

	// 旋回処理
	float turnSpeed = this->turnSpeed * Timer::Instance().DeltaTime();
	// ターゲットまでのベクトルを算出
	DirectX::XMVECTOR Position = DirectX::XMLoadFloat3(&position);
	DirectX::XMVECTOR Target = DirectX::XMLoadFloat3(&target);
	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Target, Position);
	// ゼロベクトルでないなら回転処理
	DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);
	float lengthSq;
	DirectX::XMStoreFloat(&lengthSq, LengthSq);
	if (lengthSq > 0.0001f)
	{
		// ターゲットまでのベクトルを単位ベクトル化
		Vec = DirectX::XMVector3Normalize(Vec);
		// 向いている方向ベクトルを算出
		DirectX::XMVECTOR Direction = DirectX::XMLoadFloat3(&direction);
		// 前方向ベクトルとターゲットまでのベクトルの内積(角度)を算出
		DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Vec, Direction);
		float dot;
		DirectX::XMStoreFloat(&dot, Dot);
		// 2つの単位ベクトルの角度が小さいほど1.0に近づくという性質を利用して回転速度を調整する
		float rot = 1.0f - dot;
		if (rot > turnSpeed)
		{
			rot = turnSpeed;
		}
		// 回転角度があるなら回転処理をする
		if (rot > 0.0001f)
		{
			// 回転軸を算出
			DirectX::XMVECTOR Axis = DirectX::XMVector3Cross(Direction, Vec);
			// 回転軸と回転量から回転行列を算出
			DirectX::XMMATRIX Rotation = DirectX::XMMatrixRotationAxis(Axis, rot);
			// 現在の行列を回転させる
			DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);
			Transform = DirectX::XMMatrixMultiply(Rotation, Transform);
			// 回転後の前方方向を取り出し、単位ベクトル化する
			Direction = DirectX::XMVector3Normalize(Transform.r[2]);
			DirectX::XMStoreFloat3(&direction, Direction);
		}
	}

	
	// 行列更新
	UpdateTransform();
}

// 描画処理
void ProjectileHoming::Render()
{
	using namespace DirectX;

	// --- Graphics 取得 ---
	Graphics& gfx = Graphics::Instance();

	// rasterizer の設定
	gfx.SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_CULL_NONE);

	// 軸の修正
	const DirectX::XMFLOAT4X4 coordinateSystemTransform[] =
	{
		{ -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },	// 0:RHS Y-UP
		{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1	},	// 1:LHS Y-UP
		{ -1, 0, 0, 0, 0, 0, -1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },	// 2:RHS Z-UP
		{ 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },		// 3:LHS Z-UP
	};
	// scaleFactor の修正
	const float scaleFactors[] =
	{
		1.0f,
		0.01f,
	};

	// サイズの修正
	const float scaleFactor = scaleFactors[0];
	DirectX::XMMATRIX C = DirectX::XMLoadFloat4x4(&coordinateSystemTransform[1]) * DirectX::XMMatrixScaling(scaleFactor, scaleFactor, scaleFactor);

	DirectX::XMMATRIX Transform = XMLoadFloat4x4(&transform);

	DirectX::XMFLOAT4X4 world;
	DirectX::XMStoreFloat4x4(&world, C * Transform);

	model->Render(world, { 1,1,1,1 }, 0);
	sphere->Render(world, { 1,0,0,1 });
}

void ProjectileHoming::Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3 target)
{
	this->direction = direction;
	this->position = position;
	this->target = target;
}
