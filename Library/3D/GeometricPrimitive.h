#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>

class GeometricPrimitive
{
private:
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
	};
	struct Constants
	{
		DirectX::XMFLOAT4X4 world;
		DirectX::XMFLOAT4	materialColor;
	};

protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBuffer;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	UINT	vertexCount = 0;


public:
	// コンストラクタ
	GeometricPrimitive();
	// 継承後のクラスでデストラクタを呼ぶため
	virtual ~GeometricPrimitive() = default;

	// 描画
	void Render(const DirectX::XMFLOAT4X4& world, const DirectX::XMFLOAT4& materialColor);

private:
	// メッシュ生成　(継承後のクラスで作成する
	virtual void CreateMesh() = 0;
};