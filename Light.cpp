#include "Light.h"
#include "Library/ImGui/Include/imgui.h"
#include "Library/3D/DebugPrimitive.h"
#include "Library/3D/LineRenderer.h"

Light::Light(LightType lightType)
	: lightType(lightType)
{
}

// ライト情報を積む
void Light::PushLightData(Graphics::SceneConstants& cBuffer) const
{
	// 登録されている光源の情報を設定
	switch (lightType)
	{
	case LightType::Directional:
		cBuffer.directionalLightData.direction.x = direction.x;
		cBuffer.directionalLightData.direction.y = direction.y;
		cBuffer.directionalLightData.direction.z = direction.z;
		cBuffer.directionalLightData.direction.w = 0.0f;
		cBuffer.directionalLightData.color = color;
		break;
	case LightType::Point:
		if (cBuffer.pointLightCount >= Graphics::PointLightMax)
			break;
		cBuffer.pointLightData[cBuffer.pointLightCount].position.x = position.x;
		cBuffer.pointLightData[cBuffer.pointLightCount].position.y = position.y;
		cBuffer.pointLightData[cBuffer.pointLightCount].position.z = position.z;
		cBuffer.pointLightData[cBuffer.pointLightCount].position.w = 0.0f;
		cBuffer.pointLightData[cBuffer.pointLightCount].color = color;
		cBuffer.pointLightData[cBuffer.pointLightCount].range = range;
		cBuffer.pointLightCount++;
		break;
	case LightType::Spot:
		if (cBuffer.spotLightCount >= Graphics::SpotLightMax)
			break;
		cBuffer.spotLightData[cBuffer.spotLightCount].position.x = position.x;
		cBuffer.spotLightData[cBuffer.spotLightCount].position.y = position.y;
		cBuffer.spotLightData[cBuffer.spotLightCount].position.z = position.z;
		cBuffer.spotLightData[cBuffer.spotLightCount].position.w = 1.0f;
		cBuffer.spotLightData[cBuffer.spotLightCount].direction.x = direction.x;
		cBuffer.spotLightData[cBuffer.spotLightCount].direction.y = direction.y;
		cBuffer.spotLightData[cBuffer.spotLightCount].direction.z = direction.z;
		cBuffer.spotLightData[cBuffer.spotLightCount].direction.w = 0.0f;
		cBuffer.spotLightData[cBuffer.spotLightCount].color = color;
		cBuffer.spotLightData[cBuffer.spotLightCount].range = range;
		cBuffer.spotLightData[cBuffer.spotLightCount].innerCorn = innerCorn;
		cBuffer.spotLightData[cBuffer.spotLightCount].outerCorn = outerCorn;
		cBuffer.spotLightCount++;
		break;
	}
}

void Light::DrawDebugGUI()
{
	static const char* lightTypeName[] =
	{
		"Directional",
		"Point",
		"Spot"
	};

	if (ImGui::TreeNode(lightTypeName[static_cast<int>(lightType)]))
	{
		switch (lightType)
		{
		case	LightType::Directional:
		{
			if (ImGui::SliderFloat3("direction", &direction.x, -1.0f, 1.0f))
			{
				float l = sqrtf(direction.x * direction.x + direction.y * direction.y + direction.z * direction.z);
				direction.x /= l;
				direction.y /= l;
				direction.z /= l;
			}
			ImGui::ColorEdit3("color", &color.x);
			break;
		}
		case	LightType::Point:
		{
			ImGui::DragFloat3("position", &position.x);
			ImGui::ColorEdit3("color", &color.x);
			ImGui::DragFloat("range", &range, 0.1f, 0, FLT_MAX);
			break;
		}
		case	LightType::Spot:
		{
			ImGui::DragFloat3("position", &position.x);
			ImGui::DragFloat3("direction", &direction.x, 0.01f);
			DirectX::XMStoreFloat3(&direction, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&direction)));
			ImGui::ColorEdit3("color", &color.x);
			ImGui::DragFloat("range", &range, 0.1f, 0, FLT_MAX);
			ImGui::SliderFloat("innerCorn", &innerCorn, 0.0f, 1.0f);
			ImGui::SliderFloat("outerCorn", &outerCorn, 0.0f, 1.0f);
			break;
		}
		}
		ImGui::TreePop();
	}

}

// デバッグプリミティブ描画
void Light::DrawDebugPrimitive()
{
	DebugPrimitive& debugPrimitive = DebugPrimitive::Instance();
	LineRenderer& lineRenderer = LineRenderer::Instance();

	switch (lightType)
	{
	case LightType::Directional:
	{
		break;
	}
	case LightType::Point:
	{
		debugPrimitive.AddSphere(position, range, color);
		break;
	}
	case LightType::Spot:
	{
		DirectX::XMVECTOR	Direction = DirectX::XMLoadFloat3(&direction);
		float len;
		DirectX::XMStoreFloat(&len, DirectX::XMVector3Length(Direction));
		if (len <= 0.00001f)
			break;
		Direction = DirectX::XMVector3Normalize(Direction);
		//	軸算出
		DirectX::XMFLOAT3 dir;
		DirectX::XMStoreFloat3(&dir, Direction);
		DirectX::XMVECTOR Work = fabs(dir.y) == 1 ? DirectX::XMVectorSet(1, 0, 0, 0)
			: DirectX::XMVectorSet(0, 1, 0, 0);
		DirectX::XMVECTOR	XAxis = DirectX::XMVector3Cross(Direction, Work);
		DirectX::XMVECTOR	YAxis = DirectX::XMVector3Cross(XAxis, Direction);
		XAxis = DirectX::XMVector3Cross(Direction, YAxis);

		static constexpr int SplitCount = 16;
		for (int u = 0; u < SplitCount; u++)
		{
			float s = static_cast<float>(u) / static_cast<float>(SplitCount);
			float r = -DirectX::XM_PI + DirectX::XM_2PI * s;
			// 回転行列算出
			DirectX::XMMATRIX	RotationZ = DirectX::XMMatrixRotationAxis(Direction, r);
			// 線を算出
			DirectX::XMFLOAT3	OldPoint;
			{
				DirectX::XMVECTOR	Point = Direction;
				DirectX::XMMATRIX	Rotation = DirectX::XMMatrixRotationAxis(XAxis, acosf(outerCorn))
					* RotationZ;
				Point = DirectX::XMVectorMultiply(Point, DirectX::XMVectorSet(range, range, range, 0));
				Point = DirectX::XMVector3TransformCoord(Point, Rotation);
				Point = DirectX::XMVectorAdd(Point, DirectX::XMLoadFloat3(&position));
				DirectX::XMFLOAT3	pos;
				DirectX::XMStoreFloat3(&pos, Point);
				lineRenderer.AddVertex(position, color);
				lineRenderer.AddVertex(pos, color);
				OldPoint = pos;
			}
			// 球面を算出
			for (int v = 0; v <= SplitCount; ++v)
			{
				float s = static_cast<float>(v) / static_cast<float>(SplitCount);
				float a = outerCorn + (1.0f - outerCorn) * s;
				DirectX::XMVECTOR	Point = Direction;
				DirectX::XMMATRIX	Rotation = DirectX::XMMatrixRotationAxis(XAxis, acosf(a))
					* RotationZ;
				Point = DirectX::XMVectorMultiply(Point, DirectX::XMVectorSet(range, range, range, 0));
				Point = DirectX::XMVector3TransformCoord(Point, Rotation);
				Point = DirectX::XMVectorAdd(Point, DirectX::XMLoadFloat3(&position));
				DirectX::XMFLOAT3	pos;
				DirectX::XMStoreFloat3(&pos, Point);
				lineRenderer.AddVertex(OldPoint, color);
				lineRenderer.AddVertex(pos, color);
				OldPoint = pos;
			}
		}

		break;
	}
	}

}

