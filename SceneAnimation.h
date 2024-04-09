#pragma once
#include <string>
#include <algorithm>

#include "Scene.h"
#include "Library/3D/SkinnedMesh.h"

#include "Library/ImGui/Include/ImSequencer.h"
#include "Library/ImGui/Include/imgui_internal.h"
#include "Library/ImGui/Include/ImCurveEdit.h"
#include "Library/ImGui/Include/ImGuizmo.h"


#include "Library/3D/SkyMap.h"

class SceneAnimation;

#pragma region Sequence

static const char* SequencerItemTypeNames[] = { "Empty", "SE", "Sphere", "Effect"};
enum class SequencerItemType
{
	EMPTY,
	SE,
	Sphere,
	Effect
};

struct RampEdit : public ImCurveEdit::Delegate
{
	RampEdit()
	{
		mPts[0][0] = ImVec2(-10.f, 0);
		mPts[0][1] = ImVec2(20.f, 0.6f);
		mPts[0][2] = ImVec2(25.f, 0.2f);
		mPts[0][3] = ImVec2(70.f, 0.4f);
		mPts[0][4] = ImVec2(120.f, 1.f);
		mPointCount[0] = 5;

		mPts[1][0] = ImVec2(-50.f, 0.2f);
		mPts[1][1] = ImVec2(33.f, 0.7f);
		mPts[1][2] = ImVec2(80.f, 0.2f);
		mPts[1][3] = ImVec2(82.f, 0.8f);
		mPointCount[1] = 4;


		mPts[2][0] = ImVec2(40.f, 0);
		mPts[2][1] = ImVec2(60.f, 0.1f);
		mPts[2][2] = ImVec2(90.f, 0.82f);
		mPts[2][3] = ImVec2(150.f, 0.24f);
		mPts[2][4] = ImVec2(200.f, 0.34f);
		mPts[2][5] = ImVec2(250.f, 0.12f);
		mPointCount[2] = 6;
		mbVisible[0] = mbVisible[1] = mbVisible[2] = true;
		mMax = ImVec2(1.f, 1.f);
		mMin = ImVec2(0.f, 0.f);
	}
	size_t GetCurveCount()
	{
		return 3;
	}

	bool IsVisible(size_t curveIndex)
	{
		return mbVisible[curveIndex];
	}
	size_t GetPointCount(size_t curveIndex)
	{
		return mPointCount[curveIndex];
	}

	uint32_t GetCurveColor(size_t curveIndex)
	{
		uint32_t cols[] = { 0xFF0000FF, 0xFF00FF00, 0xFFFF0000 };
		return cols[curveIndex];
	}
	ImVec2* GetPoints(size_t curveIndex)
	{
		return mPts[curveIndex];
	}
	virtual ImCurveEdit::CurveType GetCurveType(size_t curveIndex) const { return ImCurveEdit::CurveSmooth; }
	virtual int EditPoint(size_t curveIndex, int pointIndex, ImVec2 value)
	{
		mPts[curveIndex][pointIndex] = ImVec2(value.x, value.y);
		SortValues(curveIndex);
		for (size_t i = 0; i < GetPointCount(curveIndex); i++)
		{
			if (mPts[curveIndex][i].x == value.x)
				return (int)i;
		}
		return pointIndex;
	}
	virtual void AddPoint(size_t curveIndex, ImVec2 value)
	{
		if (mPointCount[curveIndex] >= 8)
			return;
		mPts[curveIndex][mPointCount[curveIndex]++] = value;
		SortValues(curveIndex);
	}
	virtual ImVec2& GetMax() { return mMax; }
	virtual ImVec2& GetMin() { return mMin; }
	virtual unsigned int GetBackgroundColor() { return 0; }
	ImVec2 mPts[3][8];
	size_t mPointCount[3];
	bool mbVisible[3];
	ImVec2 mMin;
	ImVec2 mMax;
private:
	void SortValues(size_t curveIndex)
	{
		auto b = std::begin(mPts[curveIndex]);
		auto e = std::begin(mPts[curveIndex]) + GetPointCount(curveIndex);
		std::sort(b, e, [](ImVec2 a, ImVec2 b) { return a.x < b.x; });

	}
};

struct MySequence : public ImSequencer::SequenceInterface
{
	// interface with sequencer

	virtual int GetFrameMin() const {
		return mFrameMin;
	}
	virtual int GetFrameMax() const {
		return mFrameMax;
	}
	virtual int GetItemCount() const { return (int)myItems.size(); }

	virtual int GetItemTypeCount() const { return sizeof(SequencerItemTypeNames) / sizeof(char*); }
	virtual const char* GetItemTypeName(int typeIndex) const { return SequencerItemTypeNames[typeIndex]; }
	virtual const char* GetItemLabel(std::string name, int index) const
	{
		static char tmps[512];
		snprintf(tmps, 512, "[%s] %s", SequencerItemTypeNames[myItems[index].mType], name.c_str());
		return tmps;
	}

	virtual void Get(int index, int** start, int** end, std::string& name, int* type, unsigned int* color)
	{
		MySequenceItem& item = myItems[index];
		if (color)
			*color = 0xFFAA8080; // same color for everyone, return color based on type
		if (start)
			*start = &item.mFrameStart;
		if (end)
			*end = &item.mFrameEnd;
		if (type)
			*type = item.mType;

		name = item.name.c_str();
	}
	virtual void Add(int type) { 
		int indexNum = 0;
		for (auto myItem : myItems)
		{
			if (static_cast<int>(myItem.mType) == type)
			{
				indexNum++;
			}
		}

		myItems.push_back(MySequenceItem{ indexNum, "test", type, 0, 10});
	};
	void Add(std::string name, int type, int startFrame, int endFrame) {
		int indexNum = 0;
		for (auto myItem : myItems)
		{
			if (static_cast<int>(myItem.mType) == type)
			{
				indexNum++;
			}
		}

		myItems.push_back(MySequenceItem{ indexNum, name, type, startFrame, endFrame });
	};
	virtual void Del(int index) { myItems.erase(myItems.begin() + index); }
	virtual void Duplicate(int index) { myItems.push_back(myItems[index]); }


	// my datas
	MySequence() : mFrameMin(0), mFrameMax(0) {}
	int mFrameMin, mFrameMax;
	struct MySequenceItem
	{
		int mTypeIndex;				// ���̃^�C�v�ł̃C���f�b�N�X
		std::string name;			// ���O
		int mType;					// �^�C�v�i��� SequencerItemTypeNames �Œ�`�j
		int mFrameStart, mFrameEnd;	// �J�n�E�I���t���[��
	};
	std::vector<MySequenceItem> myItems;
	RampEdit rampEdit;

	int selectItemNum;
	virtual void DoubleClick(int index)
	{
		selectItemNum = index;
	}
};

#pragma endregion




// �^�C�g���V�[��
class SceneAnimation : public Scene
{
public:
	SceneAnimation() {}
	~SceneAnimation() override {};

	// ������
	void Initialize() override;

	// �I����
	void Finalize() override;

	// �X�V
	void Update() override;

	// �`��
	void Render() override;

private:
	// �O���b�h�`��
	void DrawGrid(int subdivisions, float scale);

	// �f�o�b�O�`��
	void DrawDebugGUI();

	// �^�C�����C������
	void DebugTimeLine();

	// �M�Y������
	void DebugGizmo();

	// ���f���N���A
	void ModelClear();

	// �G�t�F�N�g�t���O�N���A
	void ClearEffectFlag();
	// ���ʉ��t���O�N���A
	void ClearSEFlag();

private:
	std::unique_ptr<SkyMap> skyMap;

	std::string fbxPath = "";

	// lightDirection
	DirectX::XMFLOAT3 lightDirection = { 0.5,-1,-1 };
	DirectX::XMFLOAT4 lightColor = { 1,1,1,1 };
	DirectX::XMFLOAT4 ambientLightColor = { 0.2f,0.2f ,0.2f, 1.0f };

	SkinnedMesh* model = nullptr;

	bool triangulate = false;

	DirectX::XMFLOAT4X4		transform =
	{
		1,0,0,0,
		0,1,0,0,
		0,0,1,0,
		0,0,0,1
	};
	float modelAlpha = 1.0f;
	float modelScale = 1.0f;
	DirectX::XMFLOAT3 modelAngle = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 modelPosition = { 0.0f, 0.0f, 0.0f };

	// �V�[�N�G���X
	MySequence mySequence;
	
	// �A�j���[�V�����ύX�㖼�O�ꎞ�ۑ��ϐ�
	std::string newSequenceName;

	// ���ݑI�𒆂̃A�j���[�V�����ԍ�
	int animationClipIndex = 0;
	// ���݂̃t���[��(int)
	int currentFrameInt = 0;
	// ���݂̃t���[��(float)
	float currentFrameFloat = 0.0f;
	
	// ���[�v���邩 
	bool isLoop = false;
	// �Đ�����
	bool isPlay = false;
	// �Đ����x�X�P�[��
	float timeScale = 1.0f;

	// let's create the sequencer
	int selectedEntry = -1;
	int firstFrame = 0;
	bool expanded = true;
};
