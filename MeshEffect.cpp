#include "MeshEffect.h"

// 行列更新処理
void MeshEffect::UpdateTransform()
{
	// スケール行列を作成
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	// 回転行列を作成
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(DirectX::XMConvertToRadians(angle.x), DirectX::XMConvertToRadians(angle.y), DirectX::XMConvertToRadians(angle.z));
	// 位置行列を作成
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	// ４つの行列を組み合わせ、ワールド行列を作成
	DirectX::XMMATRIX W = S * R * T;
	// 計算したワールド行列を取り出す
	DirectX::XMStoreFloat4x4(&transform, W);
}
