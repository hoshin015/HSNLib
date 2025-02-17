#pragma once
#include "GeometricPrimitive.h"

class SpherePrimitive : public GeometricPrimitive
{
public:
	SpherePrimitive() {}
	SpherePrimitive(float radius);
	~SpherePrimitive() {}

	// 球
	float radius = 1.0f;
	UINT slices = 16.0f;
	UINT stacks = 16.0f;

private:

	// メッシュ生成
	void CreateMesh() override;
};