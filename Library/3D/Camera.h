#pragma once
#include <memory>
#include <DirectXMath.h>
#include "SpherePrimitive.h"
using namespace DirectX;


// TODO : いずれManagerを作って複数管理する
class Camera
{
private:
	Camera();
	~Camera() {}

public:
	static Camera& Instance()
	{
		static Camera instance;
		return instance;
	}

	// 更新
	void Update();

	void FreeCameraUpdate();
	void TargetCameraUpdate();
	void AnimationEditorCameraUpdate();
	void LockCameraUpdate();

	// ターゲット位置設定
	void SetTarget(const DirectX::XMFLOAT3& target) { this->target = target; }

	// view 設定
	void SetLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up);

	// projection 設定
	void SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ);

	// デバッグ用GUI描画
	void DrawDebugGui();

	// --- ゲッター ---

	// View 取得
	const DirectX::XMFLOAT4X4& GetView() const { return view; }
	// Projection 取得
	const DirectX::XMFLOAT4X4& GetProjection() const { return projection; }

	// 視点取得
	const DirectX::XMFLOAT3& GetEye() const { return eye; }
	// 注視点取得
	const DirectX::XMFLOAT3& GetFocus() const { return focus; }
	// 上方向取得
	const DirectX::XMFLOAT3& GetUp() const { return up; }
	// 前方向取得
	const DirectX::XMFLOAT3& GetFront() const { return front; }
	// 右方向取得
	const DirectX::XMFLOAT3& GetRight() const { return right; }

	// angle 設定
	void SetAngle(DirectX::XMFLOAT3 angle) { this->angle = angle; }

public:
	// デバッグプリミティブ
	std::unique_ptr<SpherePrimitive> focusSphere;
	bool drawFocusSphere = false;

	enum CAMERA
	{
		TARGET_PLAYER,
		FREE,
		ANIMATION_EDITOR,
		LOCK,
	};
	int cameraType;

private:
	// View Projection 行列
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;

	// カメラの位置
	DirectX::XMFLOAT3 eye;
	// カメラの注視点
	DirectX::XMFLOAT3 focus;
	// カメラのターゲット位置
	DirectX::XMFLOAT3 target;

	// カメラの方向ベクトル
	DirectX::XMFLOAT3 up;		// 上方向ベクトル
	DirectX::XMFLOAT3 front;	// 前方向ベクトル
	DirectX::XMFLOAT3 right;	// 右方向ベクトル

	// カメラの回転
	DirectX::XMFLOAT3	angle;


	float				maxAngleX = DirectX::XMConvertToRadians(30);
	float				minAngleX = DirectX::XMConvertToRadians(-5);
	float				limitAngleX = DirectX::XMConvertToRadians(89);

	// カーソル座標
	float oldCursorX = 0.0f;
	float oldCursorY = 0.0f;
	float difX = 0.0f;
	float difY = 0.0f;
};