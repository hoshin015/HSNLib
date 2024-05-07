#pragma once
#include "Library/3D/Sprite3D.h"
#include "Library/2D/Sprite.h"
#include "Library/2D/Primitive2D.h"

#include "Scene.h"
#include "StageContext.h"
#include "StMenuPlayer.h"
#include "Collision.h"
#include "MaskPrimitive.h"

#include <memory>

class Sprite3DObject :public Sprite3D {
public:
	Sprite3DObject(const wchar_t* filePath) :
		Sprite3D(filePath){
		_aspectRatio = { 1,float(texture2dDesc.Height) / texture2dDesc.Width };
	}

	void Draw() {
		UpdateTransform();
		Render(_transform, _color);
	}

	//四角形X軸とZ軸のみ
	bool RectHitToPoint(DirectX::XMFLOAT3 point) {
		using namespace DirectX;
		XMFLOAT2 lt = { -_scale.x * .5f ,-_scale.y * .5f};
		XMFLOAT2 rb = { +_scale.x * .5f ,+_scale.y * .5f};

		XMFLOAT3 localPoint;
		XMStoreFloat3(&localPoint, XMVector3Transform(XMLoadFloat3(&point) - XMLoadFloat3(&_position),XMMatrixRotationY(-_angle.y)));

		return (localPoint.x > lt.x && localPoint.x < rb.x) && (localPoint.z > lt.y && localPoint.z < rb.y);
	}

	bool CircleHitToPoint(DirectX::XMFLOAT3 point) {
		using namespace DirectX;
		float direction;
		direction = XMVectorGetZ(XMVector3Length(XMLoadFloat3(&point) - XMLoadFloat3(&_position)));

		return (direction < _scale.x * .5f);
	}

	void SetPosition(DirectX::XMFLOAT3 pos) { _position = pos; }
	void SetAngle(DirectX::XMFLOAT3 angle) {  _angle = angle; }
	void SetScale(DirectX::XMFLOAT3 scale) { _scale = scale; }
	void SetScaleInAsp(float scale) {
		_scale = { _aspectRatio.x * scale,_aspectRatio.y * scale,0 };
		_scaleAsp = scale;
	}
	void SetColor(DirectX::XMFLOAT4 color) { _color = color; }

	DirectX::XMFLOAT3& GetPosition() { return _position; }
	DirectX::XMFLOAT3& GetAngle() { return _angle; }
	DirectX::XMFLOAT3& GetScale() { return _scale; }
	DirectX::XMFLOAT4& GetColor() { return _color; }
	float& GetScaleAsp() { return _scaleAsp; }

private:
	void UpdateTransform() {
		// スケール行列を作成
		DirectX::XMMATRIX S = DirectX::XMMatrixScaling(_scale.x,_scale.y,_scale.z);
		// 回転行列を作成
		DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(_angle.x, _angle.y, _angle.z);
		// 位置行列を作成
		DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(_position.x, _position.y, _position.z);

		// ４つの行列を組み合わせ、ワールド行列を作成
		DirectX::XMMATRIX W = S * R * T;
		// 計算したワールド行列を取り出す
		DirectX::XMStoreFloat4x4(&_transform, W);
	}

private:
	DirectX::XMFLOAT4X4 _transform = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
	DirectX::XMFLOAT3 _position = { 0,0.05f,0 };
	DirectX::XMFLOAT3 _angle = { DirectX::XM_PIDIV2,DirectX::XM_PI,0 };
	DirectX::XMFLOAT3 _scale = { 1,1,1 };
	DirectX::XMFLOAT4 _color = { 1,1,1,1 };
	DirectX::XMFLOAT2 _aspectRatio;
	float _scaleAsp = 1;

};

class SceneSTMainMenu : public Scene {
public:
	SceneSTMainMenu() {}
	~SceneSTMainMenu() override {};

	// 初期化
	void Initialize() override;

	// 終了化
	void Finalize() override;

	// 更新
	void Update() override;

	// 描画
	void Render() override;

	// デバッグ描画
	void DrawDebugGUI();

private:
	std::vector<std::shared_ptr<Sprite3DObject>> S3DObject;
	std::vector<std::shared_ptr<Sprite>> sprite;
	//std::vector<Primitive2D> mask;
	std::unique_ptr<StageContext> stageMain;
	StMenuPlayer player;
	MaskPrimitive mask;

	bool isPlayerMove = false;
};
