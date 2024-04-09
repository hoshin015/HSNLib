#include "ProjectileHoming.h"
#include "Library/Timer.h"
#include "Library/MemoryLeak.h"

// �R���X�g���N�^
ProjectileHoming::ProjectileHoming(ProjectileManager* manager) : Projectile(manager)
{
	model = new_ SkinnedMesh("Data/Fbx/Sword/Sword.fbx", true);

	scale.x = scale.y = scale.z = 3.0f;
}

// �f�X�g���N�^
ProjectileHoming::~ProjectileHoming()
{
	delete model;
}

// �X�V����
void ProjectileHoming::Update()
{
	// ��������
	lifeTimer -= Timer::Instance().DeltaTime();
	if (lifeTimer <= 0)
	{
		Destroy();
	}

	// �ړ�
	float moveSpeed = this->moveSpeed * Timer::Instance().DeltaTime();
	position.x += direction.x * moveSpeed;
	position.y += direction.y * moveSpeed;
	position.z += direction.z * moveSpeed;

	// ���񏈗�
	float turnSpeed = this->turnSpeed * Timer::Instance().DeltaTime();
	// �^�[�Q�b�g�܂ł̃x�N�g�����Z�o
	DirectX::XMVECTOR Position = DirectX::XMLoadFloat3(&position);
	DirectX::XMVECTOR Target = DirectX::XMLoadFloat3(&target);
	DirectX::XMVECTOR Vec = DirectX::XMVectorSubtract(Target, Position);
	// �[���x�N�g���łȂ��Ȃ��]����
	DirectX::XMVECTOR LengthSq = DirectX::XMVector3LengthSq(Vec);
	float lengthSq;
	DirectX::XMStoreFloat(&lengthSq, LengthSq);
	if (lengthSq > 0.0001f)
	{
		// �^�[�Q�b�g�܂ł̃x�N�g����P�ʃx�N�g����
		Vec = DirectX::XMVector3Normalize(Vec);
		// �����Ă�������x�N�g�����Z�o
		DirectX::XMVECTOR Direction = DirectX::XMLoadFloat3(&direction);
		// �O�����x�N�g���ƃ^�[�Q�b�g�܂ł̃x�N�g���̓���(�p�x)���Z�o
		DirectX::XMVECTOR Dot = DirectX::XMVector3Dot(Vec, Direction);
		float dot;
		DirectX::XMStoreFloat(&dot, Dot);
		// 2�̒P�ʃx�N�g���̊p�x���������ق�1.0�ɋ߂Â��Ƃ��������𗘗p���ĉ�]���x�𒲐�����
		float rot = 1.0f - dot;
		if (rot > turnSpeed)
		{
			rot = turnSpeed;
		}
		// ��]�p�x������Ȃ��]����������
		if (rot > 0.0001f)
		{
			// ��]�����Z�o
			DirectX::XMVECTOR Axis = DirectX::XMVector3Cross(Direction, Vec);
			// ��]���Ɖ�]�ʂ����]�s����Z�o
			DirectX::XMMATRIX Rotation = DirectX::XMMatrixRotationAxis(Axis, rot);
			// ���݂̍s�����]������
			DirectX::XMMATRIX Transform = DirectX::XMLoadFloat4x4(&transform);
			Transform = DirectX::XMMatrixMultiply(Rotation, Transform);
			// ��]��̑O�����������o���A�P�ʃx�N�g��������
			Direction = DirectX::XMVector3Normalize(Transform.r[2]);
			DirectX::XMStoreFloat3(&direction, Direction);
		}
	}

	
	// �s��X�V
	UpdateTransform();
}

// �`�揈��
void ProjectileHoming::Render()
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
	sphere->Render(world, { 1,0,0,1 });
}

void ProjectileHoming::Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3 target)
{
	this->direction = direction;
	this->position = position;
	this->target = target;
}
