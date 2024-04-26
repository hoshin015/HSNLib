#pragma once
#include <DirectXMath.h>
#include <memory>
#include "Library/3D/SkinnedMesh.h"
#include "Library/Effekseer/Effect.h"

class MeshEffect
{
public:
	MeshEffect() {}
	virtual ~MeshEffect() {}

	// �X�V����
	virtual void Update() = 0;

	// �`�揈��
	virtual void Render() = 0;

	// �s��X�V�֐�
	void UpdateTransform();

	// �s��擾
	const DirectX::XMFLOAT4X4& GetTransform() const { return transform; }

	// �ʒu�擾
	const DirectX::XMFLOAT3& GetPosition() const { return position; }
	// �ʒu�ݒ�
	void SetPosition(const DirectX::XMFLOAT3& position) { this->position = position; }
	// ��]�擾
	const DirectX::XMFLOAT3& GetAngle() const { return angle; }
	// ��]�ݒ�
	void SetAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }
	// �X�P�[���擾
	const DirectX::XMFLOAT3& GetScale() const { return scale; }
	// �X�P�[���ݒ�
	void SetScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }
	// ���a�ݒ�
	void SetRadius(const float radius) { this->radius = radius; }

	// velocity�擾
	DirectX::XMFLOAT3 GetVelocity() { return this->velocity; }
	// velocity�ݒ�
	void SetVelocity(const DirectX::XMFLOAT3& velocity) { this->velocity = velocity; }


	// ���a�擾
	float GetRadius() const { return radius; }
	// �����擾
	float GetHeight() const { return height; }

protected:
	DirectX::XMFLOAT3		position = { 0,0,0 };
	DirectX::XMFLOAT3		angle = { 0,0,0 };
	DirectX::XMFLOAT3		scale = { 1,1,1 };
	DirectX::XMFLOAT4X4		transform =
	{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
	DirectX::XMFLOAT3 velocity = { 0,0,0 };

	float radius = 0.5f;
	float height = 1.0f;

public:
	std::shared_ptr<SkinnedMesh>	model;
};