#include "Obstacle.h"
#include "Library/Graphics/Graphics.h"
#include "Library/3D/DebugPrimitive.h"

Obstacle::Obstacle(const char* name)
{
	model = new SkinnedMesh(name);
}

Obstacle::~Obstacle()
{
	delete model;
}

// 更新処理
void Obstacle::Update()
{
	UpdateTransform();
}

// 描画処理
void Obstacle::Render()
{
	// --- Graphics 取得 ---
	Graphics& gfx = Graphics::Instance();

	// rasterizer 設定
	gfx.SetRasterizer(RASTERIZER_STATE::CLOCK_TRUE_SOLID);

	model->Render(transform, { 1, 1, 1, 1 }, 0);
}

void Obstacle::DrawDebugPrimitive()
{
	DebugPrimitive::Instance().AddSphere(position, 1.1, { 0,0,0,1 });		// スポーン地点
}

bool Obstacle::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	return Collision::IntersectRayVsModel(start, end, model, transform, hit);
	//return Collision::IntersectRayVsModel(start, end, collisionModel, hit);
}
void Obstacle::UpdateTransform()
{
	const float scaleFactor = model->scaleFactors[model->fbxUnit];
	DirectX::XMMATRIX C = DirectX::XMLoadFloat4x4(&model->coordinateSystemTransform[model->coordinateSystemIndex]) * DirectX::XMMatrixScaling(scaleFactor, scaleFactor, scaleFactor);

	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	DirectX::XMMATRIX W = C * S * R * T;
	DirectX::XMStoreFloat4x4(&transform, W);
}
