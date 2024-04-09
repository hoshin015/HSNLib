#pragma once
#include <map>
#include <memory>
#include <string>
#include "SkinnedMesh.h"

// リソースマネージャー
class ResourceManager
{
private:
	ResourceManager() {}
	~ResourceManager() {}

public:
	static ResourceManager& Instance()
	{
		static ResourceManager instance;
		return instance;
	}

	// モデルリソース読み込み
	std::shared_ptr<SkinnedMesh> LoadModelResource(const char* filename);

private:
	using ModelMap = std::map<std::string, std::weak_ptr<SkinnedMesh>>;

	ModelMap models;
};