#pragma once

#include <DirectXMath.h>
#include <Effekseer.h>

enum class EffectType
{
	Hit1,
	Move,
	PowerUp,
	Sleep,
	Test1,
	LAST,
};

// エフェクト
class Effect
{
private:
	Effect() {}
	~Effect() {}


public:
	static Effect& Instance()
	{
		static Effect instance;
		return instance;
	};


	// 初期化
	void Initialize();

	// データ作成
	void CreateEffect(EffectType index, const char* filename);

	// 再生
	Effekseer::Handle Play(EffectType index, const DirectX::XMFLOAT3& position, DirectX::XMFLOAT3 angle, float scale = 1.0f);

	// 停止
	void Stop(Effekseer::Handle handle);

	// 座標設定
	void SetPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3& position);

	// スケール設定
	void SetScale(Effekseer::Handle handle, const DirectX::XMFLOAT3& scale);

	// 角度設定
	void SetAngle(Effekseer::Handle handle, const DirectX::XMFLOAT3& angle);

private:
	Effekseer::EffectRef effekseerEffects[static_cast<int>(EffectType::LAST)];
};