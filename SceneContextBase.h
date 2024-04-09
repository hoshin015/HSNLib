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


// �^�C�g���V�[��
class SceneContextBase : public Scene
{
public:
	SceneContextBase() {}
	~SceneContextBase() override {};

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
	// �G�l�~�[HP�Q�[�W�`��
	void RenderEnemyGauge();

	// �O���b�h�`��
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

	static const int numRays = 8;		// ���C�̖{��
	float lookAhead = 10;				// ���C�̋���

	float rayDirections[numRays] = {};	// ���C�̊p�x
	float interest[numRays] = {};		// �Ή�����p�x�ɂǂꂾ���ړ���������
	float acceleration[numRays] = {};	// �Ή�����p�x�ɂǂꂾ���ړ��������Ȃ���
	
};
