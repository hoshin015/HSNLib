#include "Camera.h"
#include "../ImGui/Include/imgui.h"
#include "../Graphics/Graphics.h"
#include "../Input/InputManager.h"
#include "../Timer.h"
#include "../ImGui/ConsoleData.h"


Camera::Camera()
{
	// --- Graphics 取得 ---
	Graphics& gfx = Graphics::Instance();

	// --- プロジェクション座標変換行列作成 ---
	D3D11_VIEWPORT viewport;
	UINT numViewports{ 1 };
	gfx.deviceContext->RSGetViewports(&numViewports, &viewport);

	float aspectRatio{ viewport.Width / viewport.Height };
	SetPerspectiveFov(XMConvertToRadians(30), aspectRatio, 0.1f, 1000.0f);

	// --- ビュー座標変換行列作成 ---
	SetLookAt(
		DirectX::XMFLOAT3(0, 5, 20),		// カメラ座標
		DirectX::XMFLOAT3(-30, 0, -30),		// ターゲット(設定しても意味ない)
		DirectX::XMFLOAT3(0, 1, 0)			// 上方向ベクトル
	);

	// --- angle 設定 ---
	angle = { DirectX::XMConvertToRadians(15),DirectX::XMConvertToRadians(180), 0 };

	// --- debugPrimitive 生成 ---
	focusSphere = std::make_unique<SpherePrimitive>();
}

void Camera::Update()
{
	// --- 入力処理 ---
	InputManager& inputManager = InputManager::Instance();
	inputManager.ResetScrollWheelValue();
	
	switch (cameraType)
	{
	case CAMERA::TARGET_PLAYER:
		TargetCameraUpdate();
		break;
	case CAMERA::FREE:
		FreeCameraUpdate();
		break;
	case CAMERA::ANIMATION_EDITOR:
		AnimationEditorCameraUpdate();
		break;
	case CAMERA::LOCK:
		LockCameraUpdate();
		break;
	case CAMERA::TargetStPlayer:
		TargetStPlayerCameraUpdate();
		break;
	}
}

void Camera::FreeCameraUpdate()
{
	// --- 入力処理 ---
	InputManager& inputManager = InputManager::Instance();

	float ax = 0;
	float ay = 0;
	float az = 0;

	difX = oldCursorX - inputManager.GetCursorPosX();
	difY = oldCursorY - inputManager.GetCursorPosY();
	oldCursorX = inputManager.GetCursorPosX();
	oldCursorY = inputManager.GetCursorPosY();

	if (inputManager.GetMousePress(MOUSEBUTTON_STATE::rightButton))
	{
		angle.y -= difX * Timer::Instance().DeltaTime();
		angle.x -= difY * Timer::Instance().DeltaTime();
	}
	if (inputManager.GetMousePress(MOUSEBUTTON_STATE::middleButton))
	{
		ax += difX * Timer::Instance().DeltaTime() * 10;
		az -= difY * Timer::Instance().DeltaTime() * 10;
	}
	ay += inputManager.GetScrollWheelValue() * Timer::Instance().DeltaTime() * 10;
	inputManager.ResetScrollWheelValue();


	// --- ターゲットの座標をカメラの回転から求める　---

	// カメラ回転値を回転行列に変換
	DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

	// 回転行列から前方向ベクトルを取り出す
	DirectX::XMVECTOR Front = Transform.r[2];
	DirectX::XMFLOAT3 front;
	DirectX::XMStoreFloat3(&front, Front);

	// カメラ座標から前ベクトル方向に一定距離離れた注視点を求める
	focus.x = eye.x + front.x * 10;
	focus.y = eye.y + front.y * 10;
	focus.z = eye.z + front.z * 10;

	// --- カメラの移動ベクトル計算 ---

	// カメラの方向を取得
	DirectX::XMFLOAT3& cameraRight = right;
	DirectX::XMFLOAT3& cameraFront = front;
	DirectX::XMFLOAT3& cameraUp = up;

	// カメラ右方向ベクトルを単位ベクトルに変換
	DirectX::XMVECTOR cameraVectorRight = DirectX::XMLoadFloat3(&cameraRight);
	DirectX::XMVector3Normalize(cameraVectorRight);
	DirectX::XMStoreFloat3(&cameraRight, cameraVectorRight);

	// カメラ前方向ベクトルを単位ベクトルに変換
	DirectX::XMVECTOR cameraVectorFront = DirectX::XMLoadFloat3(&cameraFront);
	DirectX::XMVector3Normalize(cameraVectorFront);
	DirectX::XMStoreFloat3(&cameraFront, cameraVectorFront);

	// カメラ上方向ベクトルを単位ベクトルに変換
	DirectX::XMVECTOR cameraVectorUp = DirectX::XMLoadFloat3(&cameraUp);
	DirectX::XMVector3Normalize(cameraVectorUp);
	DirectX::XMStoreFloat3(&cameraUp, cameraVectorUp);

	// 進行ベクトルを計算する
	DirectX::XMFLOAT3 vec;
	vec.x = ax * cameraRight.x + ay * cameraFront.x + az * cameraUp.x;
	vec.y = ax * cameraRight.y + ay * cameraFront.y + az * cameraUp.y;
	vec.z = ax * cameraRight.z + ay * cameraFront.z + az * cameraUp.z;

	// --- ターゲットとカメラを同じだけ動かす ---
	focus.x += vec.x;
	focus.y += vec.y;
	focus.z += vec.z;

	eye.x += vec.x;
	eye.y += vec.y;
	eye.z += vec.z;

	// --- view 設定 ---
	SetLookAt(eye, focus, DirectX::XMFLOAT3(0, 1, 0));
}

void Camera::TargetCameraUpdate()
{
	// --- 入力処理 ---
	InputManager& inputManager = InputManager::Instance();

	float ax = 0;
	float ay = 0;

	if (inputManager.GetKeyPress(DirectX::Keyboard::J)) ax--;
	if (inputManager.GetKeyPress(DirectX::Keyboard::L)) ax++;
	if (inputManager.GetKeyPress(DirectX::Keyboard::I)) ay--;
	if (inputManager.GetKeyPress(DirectX::Keyboard::K)) ay++;

	difX = oldCursorX - inputManager.GetCursorPosX();
	difY = oldCursorY - inputManager.GetCursorPosY();
	oldCursorX = inputManager.GetCursorPosX();
	oldCursorY = inputManager.GetCursorPosY();

	if (inputManager.GetMousePress(MOUSEBUTTON_STATE::rightButton))
	{
		angle.y -= difX * Timer::Instance().DeltaTime() * 3;
		angle.x -= difY * Timer::Instance().DeltaTime() * 3;
	}

	// スティック入力値に合わせてX軸とY軸を回転
	angle.x += ay * 2 * Timer::Instance().DeltaTime();
	angle.y += ax * 2 * Timer::Instance().DeltaTime();

	// X軸のカメラ回転を制限
	if (angle.x > maxAngleX) angle.x = maxAngleX;
	if (angle.x < minAngleX) angle.x = minAngleX;

	// Y軸の回転値を-3.14～3.14に収まるようにする
	if (angle.y < -DirectX::XM_PI)
	{
		angle.y += DirectX::XM_2PI;
	}
	if (angle.y > DirectX::XM_PI)
	{
		angle.y -= DirectX::XM_2PI;
	}

	// カメラ回転値を回転行列に変換
	DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

	// 回転行列から前方向ベクトルを取り出す
	DirectX::XMVECTOR Front = Transform.r[2];
	DirectX::XMFLOAT3 front;
	DirectX::XMStoreFloat3(&front, Front);

	// 注視点から後ろベクトル方向に一定距離離れたカメラ視点を求める
	DirectX::XMFLOAT3 eye;
	eye.x = target.x - front.x * 15;
	eye.y = target.y - front.y * 15;
	eye.z = target.z - front.z * 15;

	// カメラの視点と注視点を設定
	SetLookAt(eye, target, DirectX::XMFLOAT3(0, 1, 0));
}

void Camera::AnimationEditorCameraUpdate()
{
	// --- 入力処理 ---
	InputManager& inputManager = InputManager::Instance();

	difX = oldCursorX - inputManager.GetCursorPosX();
	difY = oldCursorY - inputManager.GetCursorPosY();
	oldCursorX = inputManager.GetCursorPosX();
	oldCursorY = inputManager.GetCursorPosY();

	if (inputManager.GetMousePress(MOUSEBUTTON_STATE::rightButton))
	{
		angle.y -= difX * Timer::Instance().DeltaTime() * 3;
		angle.x -= difY * Timer::Instance().DeltaTime() * 3;
	}
	static float distance = 10;
	if (inputManager.GetMousePress(MOUSEBUTTON_STATE::leftButton))
	{
		distance -= inputManager.GetScrollWheelValue() * Timer::Instance().DeltaTime() * 10;
	}
	inputManager.ResetScrollWheelValue();

	// X軸のカメラ回転を制限
	if (angle.x > limitAngleX) angle.x = limitAngleX;
	if (angle.x < -limitAngleX) angle.x = -limitAngleX;

	// Y軸の回転値を-3.14～3.14に収まるようにする
	if (angle.y < -DirectX::XM_PI)
	{
		angle.y += DirectX::XM_2PI;
	}
	if (angle.y > DirectX::XM_PI)
	{
		angle.y -= DirectX::XM_2PI;
	}

	// カメラ回転値を回転行列に変換
	DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

	// 回転行列から前方向ベクトルを取り出す
	DirectX::XMVECTOR Front = Transform.r[2];
	DirectX::XMFLOAT3 front;
	DirectX::XMStoreFloat3(&front, Front);

	// 注視点から後ろベクトル方向に一定距離離れたカメラ視点を求める
	eye.x = target.x - front.x * distance;
	eye.y = target.y - front.y * distance;
	eye.z = target.z - front.z * distance;

	// カメラの視点と注視点を設定
	SetLookAt(eye, target, DirectX::XMFLOAT3(0, 1, 0));
}

void Camera::LockCameraUpdate()
{
	// --- ターゲットの座標をカメラの回転から求める　---

	// カメラ回転値を回転行列に変換
	DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

	// 回転行列から前方向ベクトルを取り出す
	DirectX::XMVECTOR Front = Transform.r[2];
	DirectX::XMFLOAT3 front;
	DirectX::XMStoreFloat3(&front, Front);

	// カメラ座標から前ベクトル方向に一定距離離れた注視点を求める
	focus.x = eye.x + front.x * 10;
	focus.y = eye.y + front.y * 10;
	focus.z = eye.z + front.z * 10;

	// --- カメラの移動ベクトル計算 ---

	// カメラの方向を取得
	DirectX::XMFLOAT3& cameraRight = right;
	DirectX::XMFLOAT3& cameraFront = front;
	DirectX::XMFLOAT3& cameraUp = up;

	// カメラ右方向ベクトルを単位ベクトルに変換
	DirectX::XMVECTOR cameraVectorRight = DirectX::XMLoadFloat3(&cameraRight);
	DirectX::XMVector3Normalize(cameraVectorRight);
	DirectX::XMStoreFloat3(&cameraRight, cameraVectorRight);

	// カメラ前方向ベクトルを単位ベクトルに変換
	DirectX::XMVECTOR cameraVectorFront = DirectX::XMLoadFloat3(&cameraFront);
	DirectX::XMVector3Normalize(cameraVectorFront);
	DirectX::XMStoreFloat3(&cameraFront, cameraVectorFront);

	// カメラ上方向ベクトルを単位ベクトルに変換
	DirectX::XMVECTOR cameraVectorUp = DirectX::XMLoadFloat3(&cameraUp);
	DirectX::XMVector3Normalize(cameraVectorUp);
	DirectX::XMStoreFloat3(&cameraUp, cameraVectorUp);

	// --- view 設定 ---
	SetLookAt(eye, focus, DirectX::XMFLOAT3(0, 1, 0));
}

void Camera::TargetStPlayerCameraUpdate()
{
	// カメラ回転値を回転行列に変換
	DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);

	// 回転行列から前方向ベクトルを取り出す
	DirectX::XMVECTOR Front = Transform.r[2];
	DirectX::XMFLOAT3 front;
	DirectX::XMStoreFloat3(&front, Front);

	// 回転行列から上方向ベクトルを取り出す
	DirectX::XMVECTOR Up = Transform.r[1];
	DirectX::XMFLOAT3 up;
	DirectX::XMStoreFloat3(&up, Up);

	// 注視点から後ろベクトル方向に一定距離離れたカメラ視点を求める
	DirectX::XMFLOAT3 eye;
	eye.x = target.x;
	eye.y = target.y + 45;
	eye.z = target.z + 30;

	// カメラの視点と注視点を設定
	SetLookAt(eye, target, DirectX::XMFLOAT3(0, 1, 0));
}

// view 設定
void Camera::SetLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up)
{
	// 視点、注視点、上方向からビュー行列を作成
	DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&eye);
	DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&focus);
	DirectX::XMVECTOR Up = DirectX::XMLoadFloat3(&up);
	DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
	DirectX::XMStoreFloat4x4(&view, View);

	// ビューを逆行列化し、ワールド行列に戻す
	DirectX::XMMATRIX World = DirectX::XMMatrixInverse(nullptr, View);
	DirectX::XMFLOAT4X4 world;
	DirectX::XMStoreFloat4x4(&world, World);

	// カメラの方向を取り出す
	this->right.x = world.m[0][0];
	this->right.y = world.m[0][1];
	this->right.z = world.m[0][2];

	this->up.x = world.m[1][0];
	this->up.y = world.m[1][1];
	this->up.z = world.m[1][2];

	this->front.x = world.m[2][0];
	this->front.y = world.m[2][1];
	this->front.z = world.m[2][2];

	// 視点、注視点を保存
	this->eye = eye;
	this->focus = focus;
}

// projection 設定
void Camera::SetPerspectiveFov(float fovY, float aspect, float nearZ, float farZ)
{
	DirectX::XMMATRIX Projection = XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);
	DirectX::XMStoreFloat4x4(&projection, Projection);
}

void Camera::DrawDebugGui()
{
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_FirstUseEver);

	if (ImGui::Begin("Camera", nullptr, ImGuiWindowFlags_None))
	{
		// --- Rasterizer ---
		{
			const char* cameraTypeName[] = { "TARGET_PLAYER", "FREE", "ANIMATION_EDITOR" };
			if (ImGui::BeginCombo("CameraType", cameraTypeName[cameraType]))
			{
				for (int i = 0; i < IM_ARRAYSIZE(cameraTypeName); i++)
				{
					const bool isSelected = (cameraType == i);
					if (ImGui::Selectable(cameraTypeName[i], isSelected))
					{
						cameraType = i;
					}
					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
		}

		// eye reset
		if (ImGui::Button("RESET EYE"))
		{
			eye = { 0,5,20 };
		}

		// drawFocusSphere
		ImGui::Checkbox("DrawFocusSphere", &drawFocusSphere);

		// 位置
		ImGui::DragFloat3("Eye", &eye.x, 0.1f);
		// focus
		ImGui::DragFloat3("Focus", &focus.x, 0.1f);
		// 回転
		DirectX::XMFLOAT3 a;
		a.x = DirectX::XMConvertToDegrees(angle.x);
		a.y = DirectX::XMConvertToDegrees(angle.y);
		a.z = DirectX::XMConvertToDegrees(angle.z);
		ImGui::DragFloat3("Angle", &a.x);
		angle.x = DirectX::XMConvertToRadians(a.x);
		angle.y = DirectX::XMConvertToRadians(a.y);
		angle.z = DirectX::XMConvertToRadians(a.z);
		// target
		ImGui::InputFloat3("Target", &target.x);
	}
	ImGui::End();
}
