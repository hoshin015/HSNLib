#include "StageBox.h"
#include "Library/MemoryLeak.h"
#include "Library/Graphics/Graphics.h"
#include "Library/3D/ResourceManager.h"


// コンストラクタ
StageBox::StageBox()
{
	model = new_ SkinnedMesh("Data/Fbx/StageBox/StageBox.fbx", true);
}

// デストラクタ
StageBox::~StageBox()
{
	delete model;
}

// 更新処理
void StageBox::Update()
{
	// 行列更新
	UpdateTransform();

}

// 描画処理
void StageBox::Render()
{
	// --- Graphics 取得 ---
	Graphics& gfx = Graphics::Instance();

	// rasterizer 設定
	gfx.SetRasterizer(RASTERIZER_STATE::CLOCK_TRUE_SOLID);

	model->Render(transform, { 1, 1, 1, 1 }, 0);
}

// レイキャスト
bool StageBox::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	return Collision::IntersectRayVsModel(start, end, model, transform, hit);
}

// 行列更新関数
void StageBox::UpdateTransform()
{
	const float scaleFactor = model->scaleFactors[model->fbxUnit];
	DirectX::XMMATRIX C = DirectX::XMLoadFloat4x4(&model->coordinateSystemTransform[model->coordinateSystemIndex]) * DirectX::XMMatrixScaling(scaleFactor, scaleFactor, scaleFactor);

	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX W = C * S * R * T;
	DirectX::XMStoreFloat4x4(&transform, W);
}
