#pragma once
#include "Scene.h"
#include "Library/Graphics/Graphics.h"
#include "Library/2D/Primitive2D.h"
#include "Library/2D/Sprite.h"
#include "Library/2D/GraphicsSpriteBatch.h"	
#include "Library/3D/GeometricPrimitive.h"
#include "Library/3D/SpherePrimitive.h"
#include "Library/3D/CylinderPrimitive.h"
#include "Library/3D/SkinnedMesh.h"
#include "Library/3D/Camera.h"
#include "Library/3D/Sprite3D.h"
#include "Light.h"
#include "Library/3D/SkyMap.h"

// タイトルシーン
class SceneGame : public Scene
{
public:
	SceneGame() {}
	~SceneGame() override {};

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
	// エネミーHPゲージ描画
	void RenderEnemyGauge();


private:

	SkyMap* skyMap = nullptr;

	// primitive2d
	Primitive2D* primitive2d = nullptr;
	bool drawPrimitive2d = false;
	DirectX::XMFLOAT2 primitive2dPosition = { 400, 200 };
	DirectX::XMFLOAT2 primitive2dSize = { 100, 100 };
	DirectX::XMFLOAT4 primitive2dColor = { 1,1,1,1 };
	float primitive2dAngle = 0;

	// sprite
	Sprite* sprite = nullptr;
	bool drawSprite = false;
	DirectX::XMFLOAT2 spritePosition = { 600, 200 };
	DirectX::XMFLOAT2 spriteSize = { 100, 100 };
	DirectX::XMFLOAT4 spriteColor = { 1,1,1,1 };
	float spriteAngle = 0;

	// Sprite3D
	Sprite3D* sprite3D = nullptr;
	DirectX::XMFLOAT4X4 s3transform = { 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1 };
	DirectX::XMFLOAT3 s3position = { 0,1,5 };
	DirectX::XMFLOAT3 s3angle = { 0,0,0 };
	DirectX::XMFLOAT3 s3scale = { 1,1,1 };

	// graphicsSpriteBatch
	GraphicsSpriteBatch* graphicsSpriteBatch = nullptr;
	bool drawGraphicsSpriteBatch = false;
	int batchCount = 195;

	// View
	DirectX::XMFLOAT4 viewEye = { 0,0,-10,0 };
	DirectX::XMFLOAT4 viewFocus = { 0,0,0,0 };
	DirectX::XMFLOAT4 viewUp = { 0,1,0,0 };

	// lightDirection
	/*std::unique_ptr<Light>	directionalLight;
	DirectX::XMFLOAT4 ambientLightColor = { 0.2f,0.2f ,0.2f, 1.0f };*/


	// state
	int rasterizerSelect = 0;

	Primitive2D* gauge = nullptr;

	float gaussianPower = 16.0f;
};
