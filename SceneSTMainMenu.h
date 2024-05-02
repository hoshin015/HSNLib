#pragma once
#include "Library/3D/Sprite3D.h"

#include "Scene.h"
#include "StageContext.h"
#include "SpinningTopPlayerManager.h"
#include "Collision.h"

#include <memory>

class Sprite3DObject :public Sprite3D {
public:
	Sprite3DObject(const wchar_t* filePath, DirectX::XMFLOAT2 sourceSize = { 1,1 }) :
		Sprite3D(filePath), _aspectRatio({ 1,sourceSize.x / sourceSize.y }) {}

	void Draw() {
		UpdateTransform();
		Render(_transform, _color);
	}

	//X����Z���̂�
	bool HitToPoint(DirectX::XMFLOAT3 point) {
		DirectX::XMFLOAT2 lt = { _position.x - _scale.y * .5f,_position.z - _scale.x * .5f };
		DirectX::XMFLOAT2 rb = { _position.x + _scale.y * .5f,_position.z + _scale.x * .5f };

		return (point.x > lt.x && point.x < rb.x) && (point.z > lt.y && point.z < rb.y);
	}

	void SetPosition(DirectX::XMFLOAT3 pos) { _position = pos; }
	void SetAngle(DirectX::XMFLOAT3 angle) {  _angle = angle; }
	void SetScale(DirectX::XMFLOAT3 scale) { _scale = scale; }
	void SetScaleInAsp(float scale) { _scale = { _aspectRatio.x * scale,_aspectRatio.y * scale,0 }; }
	void SetColor(DirectX::XMFLOAT4 color) { _color = color; }

	DirectX::XMFLOAT3& GetPosition() { return _position; }
	DirectX::XMFLOAT3& GetAngle() { return _angle; }
	DirectX::XMFLOAT3& GetScale() { return _scale; }
	DirectX::XMFLOAT4& GetColor() { return _color; }

private:
	void UpdateTransform() {
		// �X�P�[���s����쐬
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(_scale.x,_scale.y,_scale.z);
		// ��]�s����쐬
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(_angle.x, _angle.y, _angle.z);
		// �ʒu�s����쐬
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(_position.x, _position.y, _position.z);

		// �S�̍s���g�ݍ��킹�A���[���h�s����쐬
		DirectX::XMMATRIX W = S * R * T;
		// �v�Z�������[���h�s������o��
		DirectX::XMStoreFloat4x4(&_transform, W);
	}

private:
	DirectX::XMFLOAT4X4 _transform = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
	DirectX::XMFLOAT3 _position = { 0,0.05f,0 };
	DirectX::XMFLOAT3 _angle = { DirectX::XM_PIDIV2,DirectX::XM_PI,0 };
	DirectX::XMFLOAT3 _scale = { 1,1,1 };
	DirectX::XMFLOAT4 _color = { 1,1,1,1 };
	DirectX::XMFLOAT2 _aspectRatio;

};

class SceneSTMainMenu : public Scene {
public:
	SceneSTMainMenu() {}
	~SceneSTMainMenu() override {};

	// ������
	void Initialize() override;

	// �I����
	void Finalize() override;

	// �X�V
	void Update() override;

	// �`��
	void Render() override;

	// �f�o�b�O�`��
	void DrawDebugGUI();

private:
	std::unique_ptr<Sprite3DObject> tutorialS3D;
	std::unique_ptr<StageContext> stageMain;
	StPlayer player;

};
