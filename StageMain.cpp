#include "StageMain.h"
#include "Library/MemoryLeak.h"
#include "Library/Graphics/Graphics.h"
#include "Library/3D/ResourceManager.h"


// �R���X�g���N�^
StageMain::StageMain()
{
	model = new_ SkinnedMesh("Data/Fbx/ExampleStage/ExampleStage.fbx", true);
	//model = new_ SkinnedMesh("Data/Fbx/MyStage/MyStage.fbx");
	//collisionModel = new_ SkinnedMesh("Data/Fbx/MyStageCollision/MyStageCollision.fbx");

	model->isBloomConstants.objectType = 1;
}

// �f�X�g���N�^
StageMain::~StageMain()
{
	delete model;
	//delete collisionModel;
}

// �X�V����
void StageMain::Update()
{
}

// �`�揈��
void StageMain::Render()
{
	// --- Graphics �擾 ---
	Graphics& gfx = Graphics::Instance();

	// rasterizer �ݒ�
	gfx.SetRasterizer(RASTERIZER_STATE::CLOCK_TRUE_SOLID);


	// �T�C�Y�̏C��
	const float scaleFactor = model->scaleFactors[model->fbxUnit];
	DirectX::XMMATRIX C = DirectX::XMLoadFloat4x4(&model->coordinateSystemTransform[model->coordinateSystemIndex]) * DirectX::XMMatrixScaling(scaleFactor, scaleFactor, scaleFactor);

	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(1, 1, 1);
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(0, 0, 0);
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(0, 0, 0);

	DirectX::XMFLOAT4X4 world;
	DirectX::XMStoreFloat4x4(&world, C * S * R * T);

	model->Render(world, {1, 1, 1, 1 }, 0);
}

// ���C�L���X�g
bool StageMain::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	return Collision::IntersectRayVsModel(start, end, model, hit);
	//return Collision::IntersectRayVsModel(start, end, collisionModel, hit);
}
