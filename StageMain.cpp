#include "StageMain.h"
#include "Library/MemoryLeak.h"
#include "Library/Graphics/Graphics.h"
#include "Library/3D/ResourceManager.h"


// コンストラクタ
StageMain::StageMain()
{
	model = new_ SkinnedMesh("Data/Fbx/ExampleStage/ExampleStage.fbx", true);
	//model = new_ SkinnedMesh("Data/Fbx/MyStage/MyStage.fbx");
	//collisionModel = new_ SkinnedMesh("Data/Fbx/MyStageCollision/MyStageCollision.fbx");

	model->isBloomConstants.objectType = 1;
}

// デストラクタ
StageMain::~StageMain()
{
	delete model;
	//delete collisionModel;
}

// 更新処理
void StageMain::Update()
{
}

// 描画処理
void StageMain::Render()
{
	// --- Graphics 取得 ---
	Graphics& gfx = Graphics::Instance();

	// rasterizer 設定
	gfx.SetRasterizer(RASTERIZER_STATE::CLOCK_TRUE_SOLID);


	// サイズの修正
	const float scaleFactor = model->scaleFactors[model->fbxUnit];
	DirectX::XMMATRIX C = DirectX::XMLoadFloat4x4(&model->coordinateSystemTransform[model->coordinateSystemIndex]) * DirectX::XMMatrixScaling(scaleFactor, scaleFactor, scaleFactor);

	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(1, 1, 1);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(0, 0, 0);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(0, 0, 0);

	DirectX::XMFLOAT4X4 world;
	DirectX::XMStoreFloat4x4(&world, C * S * R * T);

	model->Render(world, {1, 1, 1, 1 }, 0);
}

// レイキャスト
bool StageMain::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	return Collision::IntersectRayVsModel(start, end, model, hit);
	//return Collision::IntersectRayVsModel(start, end, collisionModel, hit);
}
