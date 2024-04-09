#pragma once
#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <fbxsdk.h>
#include <unordered_map>
#include "../Effekseer/Effect.h"
#include "../Audio/AudioManager.h"

//--------------------------------------------------------------
// Cereal
//--------------------------------------------------------------
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/set.hpp>
#include <cereal/types/unordered_map.hpp>

namespace DirectX
{
	template<class T>
	void serialize(T& archive, DirectX::XMFLOAT2& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y)
		);
	}

	template<class T>
	void serialize(T& archive, DirectX::XMFLOAT3& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y),
			cereal::make_nvp("z", v.z)
		);
	}

	template<class T>
	void serialize(T& archive, DirectX::XMFLOAT4& v)
	{
		archive(
			cereal::make_nvp("x", v.x),
			cereal::make_nvp("y", v.y),
			cereal::make_nvp("z", v.z),
			cereal::make_nvp("w", v.w)
		);
	}

	template<class T>
	void serialize(T& archive, DirectX::XMFLOAT4X4& m)
	{
		archive(
			cereal::make_nvp("_11", m._11), cereal::make_nvp("_12", m._12),
			cereal::make_nvp("_13", m._13), cereal::make_nvp("_14", m._14),
			cereal::make_nvp("_21", m._21), cereal::make_nvp("_22", m._22),
			cereal::make_nvp("_23", m._23), cereal::make_nvp("_24", m._24),
			cereal::make_nvp("_31", m._31), cereal::make_nvp("_32", m._32),
			cereal::make_nvp("_33", m._33), cereal::make_nvp("_34", m._34),
			cereal::make_nvp("_41", m._41), cereal::make_nvp("_42", m._42),
			cereal::make_nvp("_43", m._43), cereal::make_nvp("_44", m._44)
		);
	}
}


//--------------------------------------------------------------
// SkinnedScene
//--------------------------------------------------------------
struct SkinnedScene
{
	struct Node
	{
		uint64_t uniqueId =  0;
		std::string name;
		FbxNodeAttribute::EType attribute{ FbxNodeAttribute::EType::eUnknown };
		int64_t parentIndex = -1;

		// cereal
		template<class Node>
		void serialize(Node& archive)
		{
			archive(CEREAL_NVP(uniqueId), CEREAL_NVP(name), CEREAL_NVP(attribute), CEREAL_NVP(parentIndex));
		}
	};
	std::vector<Node> nodes;

	// cereal
	template<class SkinnedScene>
	void serialize(SkinnedScene& archive)
	{
		archive(CEREAL_NVP(nodes));
	}

	// node構造体vector の index を返す関数
	int64_t indexof(uint64_t uniqueId) const
	{
		int64_t index{ 0 };
		// すべての node構造体をチェック
		for (const Node& node : nodes)
		{
			if (node.uniqueId == uniqueId)
			{
				return index;
			}
			++index;
		}
		return -1;
	};
};

//--------------------------------------------------------------
// Skeleton
//--------------------------------------------------------------
struct Skeleton
{
	struct Bone
	{
		uint64_t uniqueId = 0;
		std::string name;
		int64_t parentIndex = -1;
		int64_t nodeIndex = 0;
		float radius = 0.5f;	// 当たり判定用半径

		// メッシュ空間からボーン空間へ変換するオフセット行列
		DirectX::XMFLOAT4X4 offsetTransform = {
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,0,1
		};

		// cereal
		template<class Bone>
		void serialize(Bone& archive)
		{
			archive(CEREAL_NVP(uniqueId), CEREAL_NVP(name), CEREAL_NVP(parentIndex), CEREAL_NVP(nodeIndex), CEREAL_NVP(offsetTransform));
		}

		bool isOrphan() const { return parentIndex < 0; }
	};
	std::vector<Bone> bones;

	// cereal
	template<class Skeleton>
	void serialize(Skeleton& archive)
	{
		archive(bones);
	}

	int64_t indexof(uint64_t uniqueId)
	{
		int64_t index = 0;
		for (const Bone& bone : bones)
		{
			if (bone.uniqueId == uniqueId)
			{
				return index;
			}
			index++;
		}
		return -1;
	}
};

//--------------------------------------------------------------
// SkeletonSphere
//--------------------------------------------------------------
struct SkeletonSphere
{
	std::string name;		// ノードの名前
	float radius = 1.0f;	// 当たり判定の半径

	// cereal
	template<class SkeletonSphere>
	void serialize(SkeletonSphere& archive)
	{
		archive(CEREAL_NVP(name), CEREAL_NVP(radius));
	}
};

//--------------------------------------------------------------
// CollisionSphere
//--------------------------------------------------------------
struct CollisionSphere
{
	std::string name;			
	int startFrame;
	int endFrame;
	float radius = 1;
	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT4 color = { 1,0,0,1 };

	// ボーンにバインドする場合はここにボーンの名前を保存する
	std::string bindBoneName;

	// cereal
	template<class CollisionSphere>
	void serialize(CollisionSphere& archive)
	{
		archive(CEREAL_NVP(name), CEREAL_NVP(startFrame), CEREAL_NVP(endFrame), CEREAL_NVP(radius), CEREAL_NVP(position), CEREAL_NVP(color), CEREAL_NVP(bindBoneName));
	}
};

//--------------------------------------------------------------
// AnimEffect
//--------------------------------------------------------------
struct AnimEffect
{
	EffectType effectType = EffectType::Hit1;
	std::string name;
	int startFrame;
	int endFrame;
	float scale = 1;
	DirectX::XMFLOAT3 position = { 0,0,0 };
	DirectX::XMFLOAT3 angle = { 0,0,0 };
	bool isPlay = false;	// 再生されたかどうか

	// cereal
	template<class AnimSE>
	void serialize(AnimSE& archive)
	{
		archive(CEREAL_NVP(effectType), CEREAL_NVP(name), CEREAL_NVP(startFrame), CEREAL_NVP(endFrame), CEREAL_NVP(scale), CEREAL_NVP(position), CEREAL_NVP(angle));
	}
	
};

//--------------------------------------------------------------
// AnimSE
//--------------------------------------------------------------
struct AnimSE
{
	MUSIC_LABEL musicType = MUSIC_LABEL::BGM_TEST;
	std::string name;
	int startFrame;
	int endFrame;
	bool isPlay = false;	// 再生されたかどうか

	// cereal
	template<class AnimEffect>
	void serialize(AnimEffect& archive)
	{
		archive(CEREAL_NVP(musicType), CEREAL_NVP(name), CEREAL_NVP(startFrame), CEREAL_NVP(endFrame));
	}
};

//--------------------------------------------------------------
// Animation
//--------------------------------------------------------------
struct Animation
{
	std::string name;
	float samplingRate = 0;
	float secondsLength;

	struct KeyFrame
	{
		struct Node
		{
			uint64_t uniqueId = 0;
			std::string name;

			// 'global_transform'はノードのローカル空間からシーンのグローバル空間への変換に使われる。
			DirectX::XMFLOAT4X4 globalTransform =
			{
				1,0,0,0,
				0,1,0,0,
				0,0,1,0,
				0,0,0,1
			}; 

			DirectX::XMFLOAT3 scaling = { 1,1,1 };
			DirectX::XMFLOAT4 rotation = { 0,0,0,1 };
			DirectX::XMFLOAT3 translation = { 0,0,0 };

			// cereal
			template<class Node>
			void serialize(Node& archive)
			{
				archive(CEREAL_NVP(uniqueId), CEREAL_NVP(name), CEREAL_NVP(globalTransform), CEREAL_NVP(scaling), CEREAL_NVP(rotation), CEREAL_NVP(translation));
			}
		};
		std::vector<Node> nodes;

		// cereal
		template<class KeyFrame>
		void serialize(KeyFrame& archive)
		{
			archive(CEREAL_NVP(nodes));
		}
	};
	std::vector<KeyFrame> sequence;

	std::vector<CollisionSphere> spheres;
	std::vector<AnimEffect> animEffects;
	std::vector<AnimSE> animSEs;

	// cereal
	template<class Animation>
	void serialize(Animation& archive)
	{
		archive(CEREAL_NVP(name), CEREAL_NVP(samplingRate), CEREAL_NVP(secondsLength), CEREAL_NVP(sequence), CEREAL_NVP(spheres), CEREAL_NVP(animEffects), CEREAL_NVP(animSEs));
	}
};


//--------------------------------------------------------------
// CoordinateSystemTransform
//--------------------------------------------------------------
enum class CoordinateSystemTransform
{
	RHS_Y_UP,
	LHS_Y_UP,
	RHS_Z_UP,
	LHS_Z_UP,
};

//--------------------------------------------------------------
// FbxUnit
//--------------------------------------------------------------
enum class FbxUnit
{
	Centimeter,
	Meter
};



//--------------------------------------------------------------
// SkinnedMesh
//--------------------------------------------------------------
class SkinnedMesh
{
public:
	// --- 定数 ---
	static const int MAX_BONES = 256;
	static const int MAX_BONE_INFLUENCES = 4;

public:
	struct Vertex
	{
		DirectX::XMFLOAT3 position = { 0,0,0 };
		DirectX::XMFLOAT3 normal = { 0,1,0 };
		DirectX::XMFLOAT4 tangent = { 1,0,0,1 };
		DirectX::XMFLOAT2 texcoord = { 0,0 };
		float boneWeights[MAX_BONE_INFLUENCES] = { 1,0,0,0 };
		uint32_t boneIndices[MAX_BONE_INFLUENCES] = {};

		// cereal
		template<class Vertex>
		void serialize(Vertex& archive)
		{
			archive(position, normal, tangent, texcoord, boneWeights, boneIndices);
		}
	};
	struct Constants
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4 materialColor;
		DirectX::XMFLOAT4X4 boneTransforms[MAX_BONES] = { 
			{ 
				1, 0, 0, 0, 
				0, 1, 0, 0, 
				0, 0, 1, 0, 
				0, 0, 0, 1 
			} 
		};
	};
	struct IsBloomConstants
	{

		float objectType = 0;
		DirectX::XMFLOAT3 pad;
	};
	IsBloomConstants isBloomConstants;

	struct Mesh
	{
		uint64_t uniqueId  = 0;
		std::string name;

		int64_t nodeIndex = 0;

		DirectX::XMFLOAT4X4 defaultGlobalTransform =
		{
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,0,1,
		};

		struct Subset
		{
			uint64_t materialUniqueId = 0;
			std::string materialName;

			uint32_t startIndexLocation = 0;
			uint32_t indexCount = 0;

			// cereal
			template<class Subset>
			void serialize(Subset& archive)
			{
				archive((materialUniqueId), (materialName), (startIndexLocation), (indexCount));
			}
		};

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		std::vector<Subset> subsets;

		Skeleton bindPose;

		DirectX::XMFLOAT3 boundingBox[2] =
		{
			{ +D3D11_FLOAT32_MAX, +D3D11_FLOAT32_MAX, +D3D11_FLOAT32_MAX },
			{ -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX, -D3D11_FLOAT32_MAX },
		};

		// cereal
		template<class Mesh>
		void serialize(Mesh& archive)
		{
			archive((uniqueId), (name), (nodeIndex), (subsets), (defaultGlobalTransform), (bindPose), (boundingBox), (vertices), (indices));
		}

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
		friend class SkinnedMesh;
	};

	struct Material
	{
		uint64_t uniqueId = 0;
		std::string name;

		DirectX::XMFLOAT4 Ka = { 0.2f, 0.2f, 0.2f, 1.0f };
		DirectX::XMFLOAT4 Kd = { 0.8f, 0.8f, 0.8f, 1.0f };
		DirectX::XMFLOAT4 Ks = { 1.0f, 1.0f, 1.0f, 1.0f };

		std::string textureFilenames[4];
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceViews[4];

		// cereal
		template<class Material>
		void serialize(Material& archive)
		{
			archive(CEREAL_NVP(uniqueId), CEREAL_NVP(name), CEREAL_NVP(Ka), CEREAL_NVP(Kd), CEREAL_NVP(Ks), CEREAL_NVP(textureFilenames));
		}
	};

public:
	std::vector<Mesh> meshes;
	std::unordered_map<uint64_t, Material> materials;
	std::vector<Animation> animationClips;

	// 球の当たり判定
	std::vector<SkeletonSphere> skeletonSpheres;

	std::string fbxPath;
	std::string parentPath;

	// 軸の修正
	const DirectX::XMFLOAT4X4 coordinateSystemTransform[4] =
	{
		{ -1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 },	// 0:RHS Y-UP
		{ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1	},	// 1:LHS Y-UP
		{ -1, 0, 0, 0, 0, 0, -1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },	// 2:RHS Z-UP
		{ 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1 },		// 3:LHS Z-UP
	};
	int coordinateSystemIndex;

	// scaleFactor の修正
	const float scaleFactors[2] =
	{
		1.0f,
		0.01f,
	};
	int fbxUnit;
	
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> isBloomConstantBuffer;

public:
	SkinnedMesh(const char* fbxFilename, bool triangulate = false, float samplingRate = 0);
	virtual ~SkinnedMesh() = default;

	// FbxLoad処理
	void LoadFbx(const char* fbxFilename, bool triangulate, float samplingRate);

	// ModelLoad処理
	void LoadModel();



	// 描画
	void Render(const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& materialColor, const Animation::KeyFrame* keyFrame);

	// mesh 取得
	void FetchMeshes(const char* fbxFilename, FbxScene* fbxScene, std::vector<Mesh>& meshes);
	// material 取得
	void FetchMaterials(const char* fbxFilename, FbxScene* fbxScene, std::unordered_map<uint64_t, Material>& materials);
	// バインドポーズの情報 取得
	void FetchSkeleton(FbxMesh* fbxMesh, Skeleton& bindPose);
	// アニメーション情報の取得
	void FetchAnimations(const char* fbxFilename, FbxScene* fbxScene, std::vector<Animation>& animationClips, float samplingRate);

	// アニメーションの更新(アニメーションのもつ node の変換行列の更新)
	void UpdateAnimation(Animation::KeyFrame& keyFrame);
	// アニメーションの追加
	bool AppendAnimations(const char* animationFileName, float samplingRate);
	// アニメーションブレンド
	void BlendAnimations(const Animation::KeyFrame* keyFrames[2], float factor, Animation::KeyFrame& keyFrame);

	// オブジェクト生成
	void CreateComObjects(const char* fbxFilename);
	// ダミーテクスチャの生成
	HRESULT MakeDummyTexture(ID3D11ShaderResourceView** shaderResourceView, DWORD value/*0xAABBGGRR*/, UINT dimension);
	// ダミーマテリアルの作成
	void MakeDummyMaterial();

	// CoordinateSystemTransform 軸設定
	void SetCoordinateSystemTransform(CoordinateSystemTransform coordinateSystemTransform);

	// fbx の単位設定
	void SetFbxUnit(FbxUnit fbxUnit);
public:
	SkinnedScene sceneView;
};