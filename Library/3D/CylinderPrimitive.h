#pragma once
#include "GeometricPrimitive.h"

class CylinderPrimitive : public GeometricPrimitive
{
public:
	CylinderPrimitive(float radius, float height);
	~CylinderPrimitive() {}

	// ‰~’Œ
	UINT start = 0;
	float radius1 = 1.0f;
	float radius2 = 1.0f;
	float height = 1.0f;
	UINT slices = 16.0f;
	UINT stacks = 16.0f;

private:
	// ƒƒbƒVƒ…¶¬
	void CreateMesh() override;
};