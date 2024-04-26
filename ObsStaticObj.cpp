#include "ObsStaticObj.h"
#include "Library/3D/ResourceManager.h"

ObsStaticObj::ObsStaticObj(const char* filePath)
{
	model = ResourceManager::Instance().LoadModelResource(filePath);
}

ObsStaticObj::~ObsStaticObj()
{
}

void ObsStaticObj::Update()
{
	// çsóÒçXêV
	UpdateTransform();
}
