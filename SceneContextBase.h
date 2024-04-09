#pragma once
#include <vector>
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


// タイトルシーン
class SceneContextBase : public Scene
{
public:
	SceneContextBase() {}
	~SceneContextBase() override {};

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

	// グリッド描画
	void DrawGrid(int subdivisions, float scale);

private:
	// View
	DirectX::XMFLOAT4 viewEye = { 0,0,-10,0 };
	DirectX::XMFLOAT4 viewFocus = { 0,0,0,0 };
	DirectX::XMFLOAT4 viewUp = { 0,1,0,0 };

	// lightDirection
	DirectX::XMFLOAT3 lightDirection = { 0.5,-1,1 };

	// state
	int rasterizerSelect = 0;


	Primitive2D* gauge = nullptr;

	// --- ContextBase ---

	bool contextAdd = false;
	float contextPower = 1.0f;

	struct Context
	{
		bool plus;
		DirectX::XMFLOAT2 pos;
		float power;
	};
	std::vector<Context> contexts;

	static const int numRays = 8;		// レイの本数
	float lookAhead = 10;				// レイの距離

	float rayDirections[numRays] = {};	// レイの角度
	float interest[numRays] = {};		// 対応する角度にどれだけ移動したいか
	float acceleration[numRays] = {};	// 対応する角度にどれだけ移動したくないか
	
};
