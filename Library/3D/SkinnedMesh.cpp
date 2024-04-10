#include <functional>
#include <sstream>
#include <fstream>
#include <filesystem>
#include "SkinnedMesh.h"
#include "../Graphics/Graphics.h"
#include "../Graphics/Shader.h"
#include "../Graphics/Texture.h"
#include "../ImGui/ConsoleData.h"	
#include "../ErrorLogger.h"


//--------------------------------------------------------------
// ボーン影響度
//--------------------------------------------------------------

// ボーン影響度構造体
struct boneInfluence
{
	uint32_t boneIndex;	// ボーンの番号
	float boneWeight;	// そのボーンに対する追従度
};
using boneInfluencesPerControlPoint = std::vector<boneInfluence>;

// boneInfluenceの取得
void FetchBoneInfluences(const FbxMesh* fbxMesh, std::vector<boneInfluencesPerControlPoint>& boneInfluences)
{
	// --- ボーン影響度構造体のサイズをmeshの持つ頂点の数に変更 ---
	const int controlPointsCount = fbxMesh->GetControlPointsCount();
	boneInfluences.resize(controlPointsCount);

	// すべての deformer (FbxSkin) を処理 
	const int skinCount = fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int skinIndex = 0; skinIndex < skinCount; ++skinIndex)
	{
		// 処理する deformer の取得	
		const FbxSkin* fbxSkin = static_cast<FbxSkin*>(fbxMesh->GetDeformer(skinIndex, FbxDeformer::eSkin));

		// この deformer がもつ全ての cluster を処理する --- (すべてのボーンの処理)
		const int clusterCount = fbxSkin->GetClusterCount();
		for (int clusterIndex = 0; clusterIndex < clusterCount; ++clusterIndex)
		{
			// 現在の index の cluster を取得 --- (処理するボーンの取得)
			const FbxCluster* fbxCluster = fbxSkin->GetCluster(clusterIndex);

			// cluster が影響を与える頂点の数を取得(1つの cluster が３つの頂点に影響を与える場合３を返す) --- (ボーンが影響を与える全ての頂点を処理)
			const int controlPointIndicesCount = fbxCluster->GetControlPointIndicesCount();

			for (int controlPointIndicesIndex = 0; controlPointIndicesIndex < controlPointIndicesCount; ++controlPointIndicesIndex)
			{
				// index と weight を push する
				int controlPointIndex = fbxCluster->GetControlPointIndices()[controlPointIndicesIndex];
				double controlPointWeight = fbxCluster->GetControlPointWeights()[controlPointIndicesIndex];
				boneInfluence& boneInfluence = boneInfluences.at(controlPointIndex).emplace_back();
				boneInfluence.boneIndex = static_cast<uint32_t>(clusterIndex);
				boneInfluence.boneWeight = static_cast<float>(controlPointWeight);
			}
		}
	}
}

//--------------------------------------------------------------
// Fbx の行列から DirectX の行列に変換する関数
//--------------------------------------------------------------
inline XMFLOAT4X4 toXmfloat4x4(const FbxAMatrix& fbxamatrix)
{
	XMFLOAT4X4 xmfloat4x4;
	for (int row = 0; row < 4; ++row)
	{
		for (int column = 0; column < 4; ++column)
		{
			xmfloat4x4.m[row][column] = static_cast<float>(fbxamatrix[row][column]);
		}
	}
	return xmfloat4x4;
}
inline XMFLOAT3 toXmfloat3(const FbxDouble3& fbxdouble3)
{
	XMFLOAT3 xmfloat3;
	xmfloat3.x = static_cast<float>(fbxdouble3[0]);
	xmfloat3.y = static_cast<float>(fbxdouble3[1]);
	xmfloat3.z = static_cast<float>(fbxdouble3[2]);
	return xmfloat3;
}
inline XMFLOAT4 toXmfloat4(const FbxDouble4& fbxdouble4)
{
	XMFLOAT4 xmfloat4;
	xmfloat4.x = static_cast<float>(fbxdouble4[0]);
	xmfloat4.y = static_cast<float>(fbxdouble4[1]);
	xmfloat4.z = static_cast<float>(fbxdouble4[2]);
	xmfloat4.w = static_cast<float>(fbxdouble4[3]);
	return xmfloat4;
}


//--------------------------------------------------------------
// SkinnedMesh
//--------------------------------------------------------------
SkinnedMesh::SkinnedMesh(const char* fbxFilename, bool triangulate, float samplingRate)
{
	// fbxPath の保存
	fbxPath = fbxFilename;

	// parentPath の取得
	std::filesystem::path fbxFilePath(fbxFilename);
	parentPath = fbxFilePath.parent_path().string();

	// model Path作成
	std::filesystem::path path(fbxPath);
	std::string modelFilePath = parentPath + "/" + path.stem().string() + ".model";


	if (std::filesystem::exists(modelFilePath.c_str()))
	{
		LoadModel();
	}
	else
	{
		LoadFbx(fbxFilename, triangulate, samplingRate);
	}

	
	CreateComObjects(fbxFilename);

	
	std::string message = "create " + fbxFilePath.stem().string() + " finish";
	ConsoleData::Instance().logs.push_back(message);
}


// ModelLoad処理
void SkinnedMesh::LoadModel()
{
	// --- 拡張子用変数作成 ---
	std::string pathExtension = "";

	// --- sceneView のロード ---
	std::filesystem::path path(fbxPath);
	std::string modelFilePath = parentPath + "/" + path.stem().string() + ".model";
	
#if 0
	std::ifstream ifs(modelFilePath.c_str(), std::ios::binary);
	cereal::BinaryInputArchive deserialization(ifs);
#else
	std::ifstream ifs(modelFilePath.c_str(), std::ios::in);
	cereal::JSONInputArchive deserialization(ifs);
#endif
	deserialization(sceneView, coordinateSystemIndex, fbxUnit, skeletonSpheres);

	// --- メッシュ作成 ---
	std::string meshPath = parentPath + "/Mesh/";
	pathExtension = ".mesh";

	// ".mesh" の拡張子のファイルを全て処理
	for (const auto& entry : std::filesystem::directory_iterator(meshPath)) {
		if (entry.path().extension() == pathExtension) {

#if 1
			std::ifstream meshIfs(entry.path().c_str(), std::ios::binary);
			cereal::BinaryInputArchive   meshDeserialization(meshIfs);
#else
			std::ifstream meshIfs(entry.path().c_str(), std::ios::in);
			cereal::JSONInputArchive   meshDeserialization(meshIfs);
#endif
			Mesh mesh;
			meshDeserialization(mesh);
			meshes.push_back(mesh);
		}
	}

	// --- マテリアル作成 ---
	std::string materialPath = parentPath + "/Material/";
	pathExtension = ".material";

	// ".material" の拡張子のファイルを全て処理
	for (const auto& entry : std::filesystem::directory_iterator(materialPath)) {
		if (entry.path().extension() == pathExtension) {

			std::ifstream materialIfs(entry.path().c_str(), std::ios::in);
			cereal::JSONInputArchive   meshDeserialization(materialIfs);
			Material material;
			meshDeserialization(material);
			materials.emplace(material.uniqueId, std::move(material));
		}
	}

	// --- アニメーションクリップ作成 ---
	std::string animPath = parentPath + "/Anim/";
	pathExtension = ".anim";

	// ".anim" の拡張子のファイルを全て処理
	for (const auto& entry : std::filesystem::directory_iterator(animPath)) {
		if (entry.path().extension() == pathExtension) {

#if 1
			std::ifstream animIfs(entry.path().c_str(), std::ios::binary);
			cereal::BinaryInputArchive   animDeserialization(animIfs);
#else
			std::ifstream animIfs(entry.path().c_str(), std::ios::in);
			cereal::JSONInputArchive   animDeserialization(animIfs);
#endif
			Animation anim;

			animDeserialization(anim);

			// アニメーションの名前をファイル名と同じにする
			//anim.name = entry.path().stem().string();

			animationClips.push_back(anim);
		}
	}
}

// FbxLoad処理
void SkinnedMesh::LoadFbx(const char* fbxFilename, bool triangulate, float samplingRate)
{
	// FbxManager の作成
	FbxManager* fbxManager = FbxManager::Create();
	// FbxScene の作成
	FbxScene* fbxScene = FbxScene::Create(fbxManager, "");
	// FbxImporter の作成
	FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "");

	// ファイルの初期化
	bool importStatus = false;
	importStatus = fbxImporter->Initialize(fbxFilename);
	_ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());

	// ファイルのインポート
	importStatus = fbxImporter->Import(fbxScene);
	_ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());

	// 3Dモデルの不要なポリゴンの除去
	FbxGeometryConverter fbxConverter(fbxManager);
	if (triangulate)
	{
		// ポリゴンを３角形に変換する(重い処理)
		fbxConverter.Triangulate(fbxScene, true/*replace*/, false/*legacy*/);
		fbxConverter.RemoveBadPolygonsFromMeshes(fbxScene);
	}

	std::function<void(FbxNode*)> traverse
	{
		[&](FbxNode* fbxNode)
		{
			// node を emplace_back()
			SkinnedScene::Node& node{sceneView.nodes.emplace_back() };
			// node の属性取得
			node.attribute = fbxNode->GetNodeAttribute() ? fbxNode->GetNodeAttribute()->GetAttributeType() : FbxNodeAttribute::EType::eBoundary;
			// node の名前取得
			node.name = fbxNode->GetName();
			// node の id 取得
			node.uniqueId = fbxNode->GetUniqueID();
			// 親 node のインデックス取得
			node.parentIndex = sceneView.indexof(fbxNode->GetParent() ? fbxNode->GetParent()->GetUniqueID() : 0);
			// 子 node を traverse()
			for (int childIndex = 0; childIndex < fbxNode->GetChildCount(); ++childIndex)
			{
				traverse(fbxNode->GetChild(childIndex));
			}
		}
	};
	traverse(fbxScene->GetRootNode());

#if 1
	ConsoleData::Instance().logs.push_back("Load Fbx : " + static_cast<std::string>(fbxFilename));
	for (const SkinnedScene::Node& node : sceneView.nodes)
	{
		FbxNode* fbxNode{ fbxScene->FindNodeByName(node.name.c_str()) };

		std::string nodeName = fbxNode->GetName();
		uint64_t uid = fbxNode->GetUniqueID();
		uint64_t parentUid = fbxNode->GetParent() ? fbxNode->GetParent()->GetUniqueID() : 0;
		int32_t type = fbxNode->GetNodeAttribute() ? fbxNode->GetNodeAttribute()->GetAttributeType() : 0;		std::stringstream debugString;
		debugString << " : nodeName -" << nodeName << ": uId -" << uid << " : parentUid - " << parentUid << " : type - " << type << "\n";
		OutputDebugStringA(debugString.str().c_str());
		ConsoleData::Instance().logs.push_back(debugString.str());
	}
#endif

	// mesh の取得 ( 中で一緒にボーンの情報も取得している)
	FetchMeshes(fbxFilename, fbxScene, meshes);

	// material の取得
	FetchMaterials(fbxFilename, fbxScene, materials);

	// アニメーション情報の取得
	FetchAnimations(fbxFilename, fbxScene, animationClips, samplingRate);

	// 破棄
	fbxImporter->Destroy();
	fbxScene->Destroy();
	fbxManager->Destroy();


	// modelの出力ファイル作成
	std::filesystem::path path(fbxPath);
	std::string modelFilePath = parentPath + "/" + path.stem().string() + ".model";

	// 出力
#if 0
	std::ofstream ofs(modelFilePath.c_str(), std::ios::binary);
	cereal::BinaryOutputArchive serialization(ofs);
#else
	std::ofstream ofs(modelFilePath.c_str(), std::ios::out);
	cereal::JSONOutputArchive  serialization(ofs);
#endif
	serialization(sceneView, coordinateSystemIndex, fbxUnit, skeletonSpheres);
}

// 描画
void SkinnedMesh::Render(const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& materialColor, const Animation::KeyFrame* keyFrame)
{
	// --- Graphics 取得 ---
	Graphics& gfx = Graphics::Instance();

	// --- メッシュごとの描画 ---
	for (const Mesh& mesh : meshes)
	{
		uint32_t stride = sizeof(Vertex);
		uint32_t offset = 0;

		gfx.deviceContext->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		gfx.deviceContext->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		gfx.deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		gfx.deviceContext->IASetInputLayout(inputLayout.Get());

		
		//gfx.deviceContext->VSSetShader(vertexShader.Get(), nullptr, 0);
		//gfx.deviceContext->PSSetShader(pixelShader.Get(), nullptr, 0);
		
		
		Constants data;

		// アニメーションのキーフレームがあるならその姿勢を適用する
		if (keyFrame && keyFrame->nodes.size() > 0)
		{
			const Animation::KeyFrame::Node& meshNode = keyFrame->nodes.at(mesh.nodeIndex);
			XMStoreFloat4x4(&data.world, XMLoadFloat4x4(&meshNode.globalTransform) * XMLoadFloat4x4(&world));

			const size_t boneCount = mesh.bindPose.bones.size();
			_ASSERT_EXPR(boneCount < MAX_BONES, L"The value of the 'boneCount' has exceeded MAX_BONES.");

			for (size_t boneIndex = 0; boneIndex < boneCount; boneIndex++)
			{
				const Skeleton::Bone& bone = mesh.bindPose.bones.at(boneIndex);
				const Animation::KeyFrame::Node& boneNode = keyFrame->nodes.at(bone.nodeIndex);
				XMStoreFloat4x4(
					&data.boneTransforms[boneIndex],
					XMLoadFloat4x4(&bone.offsetTransform) *
					XMLoadFloat4x4(&boneNode.globalTransform) *
					XMMatrixInverse(nullptr, XMLoadFloat4x4(&mesh.defaultGlobalTransform))
				);
			}
		}
		else
		{
			XMStoreFloat4x4(&data.world, XMLoadFloat4x4(&mesh.defaultGlobalTransform) * XMLoadFloat4x4(&world));
			for (size_t boneIndex = 0; boneIndex < MAX_BONES; boneIndex++)
			{
				data.boneTransforms[boneIndex] =
				{
					1,0,0,0,
					0,1,0,0,
					0,0,1,0,
					0,0,0,1
				};
			}
		}
		for (const Mesh::Subset& subset : mesh.subsets)
		{
			const Material& material = materials.at(subset.materialUniqueId);
			XMStoreFloat4(&data.materialColor, XMLoadFloat4(&materialColor) * XMLoadFloat4(&material.Kd));

			gfx.deviceContext->UpdateSubresource(constantBuffer.Get(), 0, 0, &data, 0, 0);
			gfx.deviceContext->VSSetConstantBuffers(0, 1, constantBuffer.GetAddressOf());

			// isBloom
			gfx.deviceContext->UpdateSubresource(isBloomConstantBuffer.Get(), 0, 0, &isBloomConstants, 0, 0);
			gfx.deviceContext->PSSetConstantBuffers(2, 1, isBloomConstantBuffer.GetAddressOf());

			gfx.deviceContext->PSSetShaderResources(0, 1, material.shaderResourceViews[0].GetAddressOf());
			gfx.deviceContext->PSSetShaderResources(1, 1, material.shaderResourceViews[1].GetAddressOf());
			gfx.deviceContext->PSSetShaderResources(2, 1, material.shaderResourceViews[2].GetAddressOf());
			gfx.deviceContext->PSSetShaderResources(3, 1, material.shaderResourceViews[3].GetAddressOf());

			gfx.deviceContext->DrawIndexed(subset.indexCount, subset.startIndexLocation, 0);
		}
	}
}

// mesh 取得
void SkinnedMesh::FetchMeshes(const char* fbxFilename, FbxScene* fbxScene, std::vector<Mesh>& meshes)
{
	// ------- cereal処理 -------

	// Mesh フォルダの作成とパス作成
	std::filesystem::path filename(fbxFilename);	// fbx と同じ path に保存するために fbxの path で初期化
	std::string parentPath = filename.parent_path().string();	// path取得
	parentPath += "/Mesh";
	std::filesystem::create_directory(parentPath);

	// すべての node構造体 をチェック
	for (const SkinnedScene::Node& node : sceneView.nodes)
	{
		// node構造体 のタイプが mesh 以外なら continue
		if (node.attribute != FbxNodeAttribute::EType::eMesh)
		{
			continue;
		}
		// fbxNode の取得
		FbxNode* fbxNode = fbxScene->FindNodeByName(node.name.c_str());
		// fbxMesh の取得
		FbxMesh* fbxMesh = fbxNode->GetMesh();

		// mesh構造体を作成し meshes に push
		Mesh& mesh = meshes.emplace_back();
		// ↓ nodes.uniqueId をいれているの同じ？
		mesh.uniqueId = fbxMesh->GetNode()->GetUniqueID();	// mesh の親 node の id 取得
		mesh.name = fbxMesh->GetNode()->GetName();			// mesh の親 node の 名前取得
		mesh.nodeIndex = sceneView.indexof(mesh.uniqueId);	// このノードが sceneView の nodes ベクターのどこにあるかを示す
		mesh.defaultGlobalTransform = toXmfloat4x4(fbxMesh->GetNode()->EvaluateGlobalTransform());	// モデル空間の原点からみた姿勢

		// bone 処理
		std::vector<boneInfluencesPerControlPoint> boneInfluences;	// ボーンの情報をもっている
		FetchBoneInfluences(fbxMesh, boneInfluences);	// ボーンの情報(index と weight)をboneInfluencesにセット
		FetchSkeleton(fbxMesh, mesh.bindPose);

		// mesh の持つ mesh.subsets を subsets という変数にする
		std::vector<Mesh::Subset>& subsets = mesh.subsets;
		// material の数を取得
		const int materialCount = fbxMesh->GetNode()->GetMaterialCount();
		// subsets のサイズをマテリアルの数にする
		subsets.resize(materialCount > 0 ? materialCount : 1);

		for (int materialIndex = 0; materialIndex < materialCount; ++materialIndex)
		{
			// index の material を取得
			const FbxSurfaceMaterial* fbxMaterial = fbxMesh->GetNode()->GetMaterial(materialIndex);
			// subsets に 名前と id をセット
			subsets.at(materialIndex).materialName = fbxMaterial->GetName();
			subsets.at(materialIndex).materialUniqueId = fbxMaterial->GetUniqueID();
		}
		if (materialCount > 0)
		{
			// mesh のポリゴンの数を取得
			const int polygonCount = fbxMesh->GetPolygonCount();
			for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex)
			{
				// ポリゴンインデックスに対応する material の index を取得
				// ポリゴンのindexを渡すとmatrialのindexを返してくれる
				// ポリゴンx3だけindexCountを加算する
				const int materialIndex = fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(polygonIndex);
				// 対応する materialIndex の indexCount を頂点の数(3)加算する
				subsets.at(materialIndex).indexCount += 3;
			}
			// subsets 内の startIndexLocation の調整
			uint32_t offset{ 0 };
			for (Mesh::Subset& subset : subsets)
			{
				subset.startIndexLocation = offset;
				offset += subset.indexCount;
				subset.indexCount = 0;
			}
		}
		else
		{
			// ダミーマテリアル生成
			MakeDummyMaterial();
		}

		// ポリゴン数を取得
		const int polygonCount = fbxMesh->GetPolygonCount();
		mesh.vertices.resize(polygonCount * 3LL);	// vertices のサイズをポリゴンの３倍にリサイズ
		mesh.indices.resize(polygonCount * 3LL);	// indices のサイズをポリゴンの３倍にリサイズ

		// UVSet の名前を取得
		FbxStringList uvNames;
		fbxMesh->GetUVSetNames(uvNames);

		// コントロールポイント(頂点座標)を取得
		const FbxVector4* controlPoints = fbxMesh->GetControlPoints();

		// ポリゴンの数だけ繰り返す
		for (int polygonIndex = 0; polygonIndex < polygonCount; ++polygonIndex)
		{
			// ポリゴンインデックスに対応する material の index を取得
			const int materialIndex = materialCount > 0 ? fbxMesh->GetElementMaterial()->GetIndexArray().GetAt(polygonIndex) : 0;
			// 対応する materialIndex の subset を取得
			Mesh::Subset& subset = subsets.at(materialIndex);
			const uint32_t offset = subset.startIndexLocation + subset.indexCount;

			// 頂点の数(3)だけ繰り返す
			for (int positionInPolygon = 0; positionInPolygon < 3; ++positionInPolygon)
			{
				// 現在の頂点の index を計算
				const int vertexIndex = polygonIndex * 3 + positionInPolygon;

				// 頂点情報(vertex)の作成
				Vertex vertex;
				const int polygonVertex = fbxMesh->GetPolygonVertex(polygonIndex, positionInPolygon);	// polygonIndex のポリゴンの positionInPolygon の頂点の index を取得
				vertex.position.x = static_cast<float>(controlPoints[polygonVertex][0]);	// index に対応した頂点座標をセット
				vertex.position.y = static_cast<float>(controlPoints[polygonVertex][1]);
				vertex.position.z = static_cast<float>(controlPoints[polygonVertex][2]);

				// 頂点に対応する ボーンのインデックスとウェイトをセットする
				const boneInfluencesPerControlPoint& influencesPerControlPoint = boneInfluences.at(polygonVertex);
				for (size_t influenceIndex = 0; influenceIndex < influencesPerControlPoint.size(); ++influenceIndex)
				{
					if (influenceIndex < MAX_BONE_INFLUENCES)
					{
						// vertex の weight と index に influencesPerControlPoint の値をセットする
						vertex.boneWeights[influenceIndex] = influencesPerControlPoint.at(influenceIndex).boneWeight;
						vertex.boneIndices[influenceIndex] = influencesPerControlPoint.at(influenceIndex).boneIndex;
					}
				}

				// mesh に法線情報があるなら vertex にセットする
				if (fbxMesh->GetElementNormalCount() > 0)
				{
					FbxVector4 normal;
					fbxMesh->GetPolygonVertexNormal(polygonIndex, positionInPolygon, normal);
					vertex.normal.x = static_cast<float>(normal[0]);
					vertex.normal.y = static_cast<float>(normal[1]);
					vertex.normal.z = static_cast<float>(normal[2]);
				}
				// mesh にUV(テクスチャ座標)情報があるなら vertex にセットする
				if (fbxMesh->GetElementUVCount() > 0)
				{
					FbxVector2 uv;
					bool unmappedUv;
					fbxMesh->GetPolygonVertexUV(polygonIndex, positionInPolygon, uvNames[0], uv, unmappedUv);
					vertex.texcoord.x = static_cast<float>(uv[0]);
					vertex.texcoord.y = 1.0f - static_cast<float>(uv[1]);
				}
				// 法線ベクトルの値のセット(法線ベクトルを持たない場合は GenerateTangentsData で生成)
				if (fbxMesh->GenerateTangentsData(0, false))
				{
					const FbxGeometryElementTangent* tangent = fbxMesh->GetElementTangent(0);
					vertex.tangent.x = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[0]);
					vertex.tangent.y = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[1]);
					vertex.tangent.z = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[2]);
					vertex.tangent.w = static_cast<float>(tangent->GetDirectArray().GetAt(vertexIndex)[3]);
				}

				// mesh の 頂点情報ベクター(vertices)に作成した頂点情報(vertex)をセットする
				mesh.vertices.at(vertexIndex) = std::move(vertex);
				// mesh の インデックス情報ベクター(indices)に現在の index をセットする
				mesh.indices.at(static_cast<size_t>(offset) + positionInPolygon) = vertexIndex;
				subset.indexCount++;
			}
		}

		// バウンディングボックスの値設定
		for (const Vertex& v : mesh.vertices)
		{
			mesh.boundingBox[0].x = std::min<float>(mesh.boundingBox[0].x, v.position.x);
			mesh.boundingBox[0].y = std::min<float>(mesh.boundingBox[0].y, v.position.y);
			mesh.boundingBox[0].z = std::min<float>(mesh.boundingBox[0].z, v.position.z);
			mesh.boundingBox[1].x = std::min<float>(mesh.boundingBox[1].x, v.position.x);
			mesh.boundingBox[1].y = std::min<float>(mesh.boundingBox[1].y, v.position.y);
			mesh.boundingBox[1].z = std::min<float>(mesh.boundingBox[1].z, v.position.z);
		}

		// ------- cereal処理 -------

		// meshの出力ファイル作成
		std::string meshFilename = parentPath + "/" + mesh.name + ".mesh";

		// 出力
#if 1
		std::ofstream ofs(meshFilename.c_str(), std::ios::binary);
		cereal::BinaryOutputArchive  serialization(ofs);
#else
		std::ofstream ofs(meshFilename.c_str(), std::ios::out);
		cereal::JSONOutputArchive  serialization(ofs);
#endif
		serialization(mesh);
	}
}

// material 取得
void SkinnedMesh::FetchMaterials(const char* fbxFilename, FbxScene* fbxScene, std::unordered_map<uint64_t, Material>& materials)
{
	// ------- cereal処理 -------

	// Material フォルダの作成とパス作成
	std::filesystem::path filename(fbxFilename);	// fbx と同じ path に保存するために fbxの path で初期化
	std::string parentPath = filename.parent_path().string();	// path取得
	parentPath += "/Material";
	std::filesystem::create_directory(parentPath);

	// node構造体 の数を取得
	const size_t nodeCount = sceneView.nodes.size();
	// すべての node構造体 をチェック
	for (size_t nodeIndex = 0; nodeIndex < nodeCount; ++nodeIndex)
	{
		// node の nodeIndex の index の node にアクセス
		const SkinnedScene::Node& node = sceneView.nodes.at(nodeIndex);
		// 取得した node と同じ FbxNode も取得
		const FbxNode* fbxNode =fbxScene->FindNodeByName(node.name.c_str());

		// node が持つ material の数を取得
		const int materialCount = fbxNode->GetMaterialCount();
		for (int materialIndex = 0; materialIndex < materialCount; ++materialIndex)
		{
			const FbxSurfaceMaterial* fbxMaterial = fbxNode->GetMaterial(materialIndex);

			// material の作成
			Material material;
			material.name = fbxMaterial->GetName();			// 名前の取得
			material.uniqueId = fbxMaterial->GetUniqueID();	// id の取得

			// Diffseプロパティを取得
			FbxProperty fbxProperty;
			fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
			if (fbxProperty.IsValid())
			{
				const FbxDouble3 color = fbxProperty.Get<FbxDouble3>();
				material.Kd.x = static_cast<float>(color[0]);
				material.Kd.y = static_cast<float>(color[1]);
				material.Kd.z = static_cast<float>(color[2]);
				material.Kd.w = 1.0f;

				// fbxProperty に関連づけられたテクスチャの取得
				const FbxFileTexture* fbxTexture = fbxProperty.GetSrcObject<FbxFileTexture>();
				material.textureFilenames[0] = fbxTexture ? fbxTexture->GetRelativeFileName() : "";
			}
			// Ambientプロパティを取得
			fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sAmbient);
			if (fbxProperty.IsValid())
			{
				const FbxDouble3 color = fbxProperty.Get<FbxDouble3>();
				material.Ka.x = static_cast<float>(color[0]);
				material.Ka.y = static_cast<float>(color[1]);
				material.Ka.z = static_cast<float>(color[2]);
				material.Ka.w = 1.0f;

				// fbxProperty に関連づけられたテクスチャの取得
				const FbxFileTexture* fbxTexture = fbxProperty.GetSrcObject<FbxFileTexture>();
				material.textureFilenames[3] = fbxTexture ? fbxTexture->GetRelativeFileName() : "";
			}
			// Specularプロパティを取得
			fbxProperty = fbxMaterial->FindProperty(FbxSurfaceMaterial::sSpecular);
			if (fbxProperty.IsValid())
			{
				const FbxDouble3 color = fbxProperty.Get<FbxDouble3>();
				material.Ks.x = static_cast<float>(color[0]);
				material.Ks.y = static_cast<float>(color[1]);
				material.Ks.z = static_cast<float>(color[2]);
				material.Ks.w = 1.0f;

				// fbxProperty に関連づけられたテクスチャの取得
				const FbxFileTexture* fbxTexture = fbxProperty.GetSrcObject<FbxFileTexture>();
				material.textureFilenames[2] = fbxTexture ? fbxTexture->GetRelativeFileName() : "";
			}

			// ------- cereal処理 -------

			// materialの出力ファイル作成
			std::string materialFilename = parentPath + "/" + material.name + ".material";

			// 出力
			std::ofstream ofs(materialFilename.c_str(), std::ios::out);
			cereal::JSONOutputArchive  serialization(ofs);
			serialization(material);


			// push
			materials.emplace(material.uniqueId, std::move(material));
		}
	}
}

// バインドポーズの情報を取得
void SkinnedMesh::FetchSkeleton(FbxMesh* fbxMesh, Skeleton& bindPose)
{
	// すべての deformer (FbxSkin) を処理 
	const int deformerCount = fbxMesh->GetDeformerCount(FbxDeformer::eSkin);
	for (int deformerIndex = 0; deformerIndex < deformerCount; ++deformerIndex)
	{
		// 処理する deformer の取得	
		FbxSkin* skin = static_cast<FbxSkin*>(fbxMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));

		// この deformer がもつ全ての cluster を処理する --- (すべてのボーンの処理)
		const int clusterCount = skin->GetClusterCount();

		// 処理しているメッシュが持つSkeletonのBoneの大きさをボーンの数にする
		bindPose.bones.resize(clusterCount);
		
		for (int clusterIndex = 0; clusterIndex < clusterCount; clusterIndex++)
		{
			// 現在の index の cluster を取得 --- (処理するボーンの取得)
			FbxCluster* cluster = skin->GetCluster(clusterIndex);

			// ボーンの情報を取得してセット
			Skeleton::Bone& bone = bindPose.bones.at(clusterIndex);
			bone.name = cluster->GetLink()->GetName();
			bone.uniqueId = cluster->GetLink()->GetUniqueID();
			bone.parentIndex = bindPose.indexof(cluster->GetLink()->GetParent()->GetUniqueID());
			bone.nodeIndex = sceneView.indexof(bone.uniqueId);

			// メッシュの初期グローバル行列を取得
			FbxAMatrix referenceGlobalInitPosition;
			cluster->GetTransformMatrix(referenceGlobalInitPosition);

			// ボーンの初期グローバル行列を取得
			FbxAMatrix clusterGlobalInitPosition;
			cluster->GetTransformLinkMatrix(clusterGlobalInitPosition);

			// メッシュ空間の頂点座標をグローバル空間の頂点座標にする行列と
			// グローバル空間の頂点座標をボーン空間の頂点座標にする行列を掛け算することで
			// メッシュ空間からボーン空間へ変換するオフセット行列を作成する
			bone.offsetTransform = toXmfloat4x4(clusterGlobalInitPosition.Inverse() * referenceGlobalInitPosition);

			// スキニング後の頂点座標 = メッシュ空間の頂点座標 * オフセット行列
		}
	}
}

// アニメーション情報の取得
void SkinnedMesh::FetchAnimations(const char* fbxFilename, FbxScene* fbxScene, std::vector<Animation>& animationClips, float samplingRate)
{
	// ------- cereal処理 -------

	// Anim フォルダの作成とパス作成
	std::filesystem::path filename(fbxFilename);	// fbx と同じ path に保存するために fbxの path で初期化
	std::string parentPath = filename.parent_path().string();	// path取得
	parentPath += "/Anim";
	std::filesystem::create_directory(parentPath);

	// 全てのアニメーションスタックの名前の取得
	FbxArray<FbxString*> animationStackNames;
	fbxScene->FillAnimStackNameArray(animationStackNames);

	// 全てのアニメーションスタックを処理する
	const int animationStackCount = animationStackNames.GetCount();

	for (int animationStackIndex = 0; animationStackIndex < animationStackCount; animationStackIndex++)
	{
		// --- ここから各アニメーションスタックに対する処理 ---
		bool isContinue = false;
		for (int i = 0; i < animationClips.size(); i++)
		{
			std::string animationStackName = animationStackNames[animationStackIndex]->Buffer();
			std::string animationName = animationClips.at(i).name;
			if (animationStackName == animationName) isContinue = true;
		}
		if (isContinue) continue;

		// animeClipの名前に アニメーションスタック名をいれる
		Animation& animationClip = animationClips.emplace_back();
		animationClip.name = animationStackNames[animationStackIndex]->Buffer();

		// index のアニメーションスタックを取得
		FbxAnimStack* animationStack = fbxScene->FindMember<FbxAnimStack>(animationClip.name.c_str());
		fbxScene->SetCurrentAnimationStack(animationStack);

		// --- サンプリングレートとサンプリング間隔の設定 ---

		// タイムモードの取得（時間の表現方法の指定）
		const FbxTime::EMode timeMode = fbxScene->GetGlobalSettings().GetTimeMode();
		// タイムモードに対応する１秒の表現をするための変数を用意
		FbxTime oneSecond;
		oneSecond.SetTime(0, 0, 1, 0, 0, timeMode);
		// サンプリングレートの指定
		animationClip.samplingRate = samplingRate > 0 ? samplingRate : static_cast<float>(oneSecond.GetFrameRate(timeMode));
		// サンプリング間隔の計算(この秒数ごとにサンプリングを行う)
		const FbxTime samplingInterval = static_cast<FbxLongLong>(oneSecond.Get() / animationClip.samplingRate);
		// アニメーションの開始時間と終了時間の取得
		const FbxTakeInfo* takeInfo = fbxScene->GetTakeInfo(animationClip.name.c_str());
		const FbxTime startTime = takeInfo->mLocalTimeSpan.GetStart();
		const FbxTime stopTime = takeInfo->mLocalTimeSpan.GetStop();

		// アニメーション終了時間の保存
		double doubleSeconds = fbxScene->GetTakeInfo(animationClip.name.c_str())->mLocalTimeSpan.GetStop().GetSecondDouble();
		animationClip.secondsLength = static_cast<float>(doubleSeconds);
		
		// --- 各キーフレームに対する処理 ---
		// アニメーションの開始時間から終了時間まで、サンプリング間隔ごとにキーフレームを生成している
		for (FbxTime time = startTime; time < stopTime; time += samplingInterval)
		{
			// キーフレームを生成してシークエンスにemplace_backしている
			Animation::KeyFrame& keyFrame = animationClip.sequence.emplace_back();
				
			// キーフレームの node の数を このモデルがもつnodeの数にする
			const size_t nodeCount = sceneView.nodes.size();
			keyFrame.nodes.resize(nodeCount);
			
			// すべての node の処理
			for (size_t nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
			{
				FbxNode* fbxNode = fbxScene->FindNodeByName(sceneView.nodes.at(nodeIndex).name.c_str());
				if (fbxNode)
				{
					Animation::KeyFrame::Node& node = keyFrame.nodes.at(nodeIndex);

					//	uniqueId と名前のセット
					node.uniqueId = fbxNode->GetUniqueID();
					node.name = fbxNode->GetName();

					// node の グローバル変換行列にいれる
					node.globalTransform = toXmfloat4x4(fbxNode->EvaluateGlobalTransform(time));

					// node ローカル の scaling rotation translation にそれぞれ変換行列をいれる
					const FbxAMatrix& localTransform = fbxNode->EvaluateLocalTransform(time);
					node.scaling = toXmfloat3(localTransform.GetS());
					node.rotation = toXmfloat4(localTransform.GetQ());
					node.translation = toXmfloat3(localTransform.GetT());
				}
			}
		}

		// ------- cereal処理 -------

		// animの出力ファイル作成
		std::string animationFilename = parentPath + "/" + animationClip.name + ".anim";

		// 出力
#if 1
		std::ofstream ofs(animationFilename.c_str(), std::ios::binary);
		cereal::BinaryOutputArchive  serialization(ofs);
#else
		std::ofstream ofs(animationFilename.c_str(), std::ios::out);
		cereal::JSONOutputArchive  serialization(ofs);
#endif
		serialization(animationClip);
	}
	// 全てのアニメーションスタック名の解放
	for (int animationStackIndex = 0; animationStackIndex < animationStackCount; animationStackIndex++)
	{
		delete animationStackNames[animationStackIndex];
	}
}


// アニメーションの更新(アニメーションのもつ node の変換行列の更新)
void SkinnedMesh::UpdateAnimation(Animation::KeyFrame& keyFrame)
{
	// node の数だけ繰り返す
	size_t nodeCount = keyFrame.nodes.size();
	for (size_t nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
	{
		// 現在の index の node を取得
		Animation::KeyFrame::Node& node = keyFrame.nodes.at(nodeIndex);

		// それぞれの値に対応する行列の作成
		XMMATRIX S = XMMatrixScaling(node.scaling.x, node.scaling.y, node.scaling.z);
		XMMATRIX R = XMMatrixRotationQuaternion(XMLoadFloat4(&node.rotation));
		XMMATRIX T = XMMatrixTranslation(node.translation.x, node.translation.y, node.translation.z);

		// 親行列があれば親の行列をかける
		int64_t parentIndex = sceneView.nodes.at(nodeIndex).parentIndex;
		XMMATRIX P = parentIndex < 0 ? XMMatrixIdentity() : XMLoadFloat4x4(&keyFrame.nodes.at(parentIndex).globalTransform);
		
		// globalTransform に値を設定
		XMStoreFloat4x4(&node.globalTransform, S * R * T * P);

		// スケルトンのボーンも更新
		int uniqueId = node.uniqueId;
	}
}

// アニメーションの追加
bool SkinnedMesh::AppendAnimations(const char* animationFileName, float samplingRate)
{
	FbxManager* fbxManager = FbxManager::Create();
	FbxScene* fbxScene = FbxScene::Create(fbxManager, "");

	FbxImporter* fbxImporter = FbxImporter::Create(fbxManager, "");
	bool importStatus = false;
	importStatus = fbxImporter->Initialize(animationFileName);
	_ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());
	importStatus = fbxImporter->Import(fbxScene);
	_ASSERT_EXPR_A(importStatus, fbxImporter->GetStatus().GetErrorString());

	FetchAnimations(animationFileName, fbxScene, animationClips, samplingRate);

	fbxManager->Destroy();

	return true;
}

// アニメーションブレンド(2つのキーフレームの情報をまぜてるイメージ)
// keyFrames の２つの情報をまぜて keyFrame にいれてる
void SkinnedMesh::BlendAnimations(const Animation::KeyFrame* keyFrames[2], float factor, Animation::KeyFrame& keyFrame)
{
	// 出力対象の keyFrame の node を合成する keyFrame の node のサイズにする
	size_t nodeCount = keyFrames[0]->nodes.size();
	keyFrame.nodes.resize(nodeCount);
	// 全ての node を処理する
	for (size_t nodeIndex = 0; nodeIndex < nodeCount; nodeIndex++)
	{
		// scale の合成
		XMVECTOR S[2] = 
		{
			XMLoadFloat3(&keyFrames[0]->nodes.at(nodeIndex).scaling),
			XMLoadFloat3(&keyFrames[1]->nodes.at(nodeIndex).scaling)
		};
		XMStoreFloat3(&keyFrame.nodes.at(nodeIndex).scaling, XMVectorLerp(S[0], S[1], factor));

		// rotation の合成
		XMVECTOR R[2] =
		{
			XMLoadFloat4(&keyFrames[0]->nodes.at(nodeIndex).rotation),
			XMLoadFloat4(&keyFrames[1]->nodes.at(nodeIndex).rotation)
		};
		XMStoreFloat4(&keyFrame.nodes.at(nodeIndex).rotation, XMQuaternionSlerp(R[0], R[1], factor));

		// translation の合成
		XMVECTOR T[2] =
		{
			XMLoadFloat3(&keyFrames[0]->nodes.at(nodeIndex).translation),
			XMLoadFloat3(&keyFrames[1]->nodes.at(nodeIndex).translation)
		};
		XMStoreFloat3(&keyFrame.nodes.at(nodeIndex).translation, XMVectorLerp(T[0], T[1], factor));
	}
}



void SkinnedMesh::CreateComObjects(const char* fbxFilename)
{
	// --- Graphics 取得 ----
	Graphics& gfx = Graphics::Instance();


	//---  vertexBuffer と indexBuffer の作成 ---
	for (Mesh& mesh : meshes)
	{
		HRESULT hr{ S_OK };
		D3D11_BUFFER_DESC bufferDesc{};
		D3D11_SUBRESOURCE_DATA subresourceData{};
		bufferDesc.ByteWidth = static_cast<UINT>(sizeof(Vertex) * mesh.vertices.size());
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;
		subresourceData.pSysMem = mesh.vertices.data();
		subresourceData.SysMemPitch = 0;
		subresourceData.SysMemSlicePitch = 0;
		hr = gfx.device->CreateBuffer(&bufferDesc, &subresourceData, mesh.vertexBuffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

		bufferDesc.ByteWidth = static_cast<UINT>(sizeof(uint32_t) * mesh.indices.size());
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		subresourceData.pSysMem = mesh.indices.data();
		hr = gfx.device->CreateBuffer(&bufferDesc, &subresourceData, mesh.indexBuffer.ReleaseAndGetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

#if 1
		// 使用した 頂点情報とインデックス情報をクリアする
		//mesh.vertices.clear();
		//mesh.indices.clear();
#endif
	}

	//--- inputLayout と vertexShader の作成 ---
	HRESULT hr = S_OK;
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[]
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
		{"BONES", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT},
	};
	CreateVsFromCso("Data/Shader/SkinnedMesh_VS.cso", vertexShader.ReleaseAndGetAddressOf(), inputLayout.ReleaseAndGetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
	//--- pixelShader の作成 ---
	CreatePsFromCso("Data/Shader/SkinnedMesh_PS.cso", pixelShader.ReleaseAndGetAddressOf());

	//--- constantBuffer の作成 ---
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth = sizeof(Constants);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = gfx.device->CreateBuffer(&bufferDesc, nullptr, constantBuffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	//--- isBloomConstantBuffer の作成 ---
	D3D11_BUFFER_DESC isBloombBufferDesc{};
	isBloombBufferDesc.ByteWidth = sizeof(IsBloomConstants);
	isBloombBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	isBloombBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = gfx.device->CreateBuffer(&isBloombBufferDesc, nullptr, isBloomConstantBuffer.ReleaseAndGetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	// material の名前に対応する テクスチャからshaderResourceViewの生成
	for (std::unordered_map<uint64_t, Material>::iterator iterator = materials.begin(); iterator != materials.end(); ++iterator)
	{
		for (size_t textureIndex = 0; textureIndex < 4; textureIndex++)
		{
			if (iterator->second.textureFilenames[textureIndex].size() > 0)
			{
				std::filesystem::path path(fbxFilename);
				path.replace_filename(iterator->second.textureFilenames[textureIndex]);
				D3D11_TEXTURE2D_DESC texture2dDesc{};
				LoadTextureFromFile(path.c_str(), iterator->second.shaderResourceViews[textureIndex].GetAddressOf(), &texture2dDesc);
			}
			else
			{
				MakeDummyTexture(iterator->second.shaderResourceViews[textureIndex].GetAddressOf(), textureIndex == 1 ? 0xFFFF7F7F : 0xFFFFFFFF, 16);
			}
		}
	}
}

// ダミーテクスチャの生成
HRESULT SkinnedMesh::MakeDummyTexture(ID3D11ShaderResourceView** shaderResourceView, DWORD value, UINT dimension)
{
	// --- Graphics 取得 ---
	Graphics& gfx = Graphics::Instance();

	//--- ダミーテクスチャの生成 ----
	HRESULT hr = S_OK;

	D3D11_TEXTURE2D_DESC texture2dDesc{};
	texture2dDesc.Width = dimension;
	texture2dDesc.Height = dimension;
	texture2dDesc.MipLevels = 1;
	texture2dDesc.ArraySize = 1;
	texture2dDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texture2dDesc.SampleDesc.Count = 1;
	texture2dDesc.SampleDesc.Quality = 0;
	texture2dDesc.Usage = D3D11_USAGE_DEFAULT;
	texture2dDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	size_t texels = dimension * dimension;
	unique_ptr<DWORD[]> sysmem{ make_unique<DWORD[]>(texels) };
	for (size_t i = 0; i < texels; ++i) sysmem[i] = value;

	D3D11_SUBRESOURCE_DATA subresourceData{};
	subresourceData.pSysMem = sysmem.get();
	subresourceData.SysMemPitch = sizeof(DWORD) * dimension;

	ComPtr<ID3D11Texture2D> texture2d;
	hr = gfx.device->CreateTexture2D(&texture2dDesc, &subresourceData, &texture2d);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc{};
	shaderResourceViewDesc.Format = texture2dDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;
	hr = gfx.device->CreateShaderResourceView(texture2d.Get(), &shaderResourceViewDesc, shaderResourceView);
	_ASSERT_EXPR(SUCCEEDED(hr), hrTrace(hr));

	return hr;
}

// ダミーマテリアルの生成
void SkinnedMesh::MakeDummyMaterial()
{
	Material material;
	material.name = "dummyMaterial";
	material.uniqueId = 0;
	material.Kd.x = 1.0f;
	material.Kd.y = 1.0f;
	material.Kd.z = 1.0f;
	material.Kd.w = 1.0f;

	materials.emplace(material.uniqueId, std::move(material));
}

// CoordinateSystemTransform 軸設定
void SkinnedMesh::SetCoordinateSystemTransform(CoordinateSystemTransform coordinateSystemTransform)
{
	this->coordinateSystemIndex = static_cast<int>(coordinateSystemTransform);

	this->fbxUnit = static_cast<int>(fbxUnit);

	// modelの出力ファイル作成
	std::filesystem::path path(fbxPath);
	std::string modelFilePath = parentPath + "/" + path.stem().string() + ".model";

	// 出力
		// 出力
#if 0
	std::ofstream ofs(modelFilePath.c_str(), std::ios::binary);
	cereal::BinaryOutputArchive serialization(ofs);
#else
	std::ofstream ofs(modelFilePath.c_str(), std::ios::out);
	cereal::JSONOutputArchive  serialization(ofs);
#endif
	serialization(sceneView, this->coordinateSystemIndex, this->fbxUnit, skeletonSpheres);
}

// fbx の単位設定
void SkinnedMesh::SetFbxUnit(FbxUnit fbxUnit)
{
	this->fbxUnit = static_cast<int>(fbxUnit);

	// modelの出力ファイル作成
	std::filesystem::path path(fbxPath);
	std::string modelFilePath = parentPath + "/" + path.stem().string() + ".model";

	// 出力
		// 出力
#if 0
	std::ofstream ofs(modelFilePath.c_str(), std::ios::binary);
	cereal::BinaryOutputArchive serialization(ofs);
#else
	std::ofstream ofs(modelFilePath.c_str(), std::ios::out);
	cereal::JSONOutputArchive  serialization(ofs);
#endif
	serialization(sceneView, this->coordinateSystemIndex, this->fbxUnit, skeletonSpheres);
}
