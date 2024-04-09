#include "ProjectileStraight.h"
#include "Library/Timer.h"
#include "Library/MemoryLeak.h"

// �R���X�g���N�^
ProjectileStraight::ProjectileStraight(ProjectileManager* manager) : Projectile(manager)
{
	model = new_ SkinnedMesh("Data/Fbx/Sword/Sword.fbx", true);
	scale.x = scale.y = scale.z = 3.0f;
}

// �f�X�g���N�^
ProjectileStraight::~ProjectileStraight()
{
	delete model;
}

// �X�V����
void ProjectileStraight::Update()
{
	// ��������
	lifeTimer -= Timer::Instance().DeltaTime();
	if (lifeTimer <= 0.0f)
	{
		// �������폜
		Destroy();
	}

	// �ړ�
	float speed = this->speed * Timer::Instance().DeltaTime();;
	position.x += direction.x * speed;
	position.y += direction.y * speed;
	position.z += direction.z * speed;

	// �s��X�V
	UpdateTransform();
}

// �`�揈��
void ProjectileStraight::Render()
{
	using namespace DirectX;

	// --- Graphics �擾 ---
	Graphics& gfx = Graphics::Instance();

	// rasterizer �̐ݒ�
	gfx.SetRasterizer(RASTERIZER_STATE::CLOCK_FALSE_CULL_NONE);

	// ���̏C��
	const DirectX::XMFLOAT4X4 coordinateSystemTransform[] =
	{
		{ -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },	// 0:RHS Y-UP
		{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1	},	// 1:LHS Y-UP
		{ -1, 0, 0, 0, 0, 0, -1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },	// 2:RHS Z-UP
		{ 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },		// 3:LHS Z-UP
	};
	// scaleFactor �̏C��
	const float scaleFactors[] =
	{
		1.0f,
		0.01f,
	};

	// �T�C�Y�̏C��
	const float scaleFactor = scaleFactors[0];
	DirectX::XMMATRIX C = DirectX::XMLoadFloat4x4(&coordinateSystemTransform[1]) * DirectX::XMMatrixScaling(scaleFactor, scaleFactor, scaleFactor);

	DirectX::XMMATRIX Transform = XMLoadFloat4x4(&transform);

	DirectX::XMFLOAT4X4 world;
	DirectX::XMStoreFloat4x4(&world, C * Transform);

	model->Render(world, { 1,1,1,1 }, 0);
	sphere->Render(world, {1,0,0,1});
}

// ����
void ProjectileStraight::Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position)
{
	this->direction = direction;
	this->position = position;
}
