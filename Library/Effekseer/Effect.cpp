#include "../Graphics/Graphics.h"
#include "Effect.h"
#include "EffectManager.h"


// 初期化
void Effect::Initialize()
{
	CreateEffect(EffectType::Hit1, "Data/Effect/Hit.efk");
	CreateEffect(EffectType::Move, "Data/Effect/move.efk");
	CreateEffect(EffectType::PowerUp, "Data/Effect/PowerUp.efk");
	CreateEffect(EffectType::Sleep, "Data/Effect/Sleep.efk");
	CreateEffect(EffectType::Test1, "Data/Effect/Test1.efk");
}

// データ作成
void Effect::CreateEffect(EffectType index, const char* filename)
{
	// エフェクトを読み込む前にロックする
	// --- マルチスレッドでEffectを作成するとdeviceContextを同時アクセスしてフリーズする可能性があるため排他制御する
	std::lock_guard<std::mutex>	lock(Graphics::Instance().GetMutex());

	// Effekseer のリソースを読み込む
	// Effekseer はUTF-16のファイルパス以外は対応していないため文字コードの変換が必要
	char16_t utf16Filename[256];
	Effekseer::ConvertUtf8ToUtf16(utf16Filename, 256, filename);

	// Effekseer::Manager を取得
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

	// Effekseerエフェクトを読み込み
	effekseerEffects[static_cast<int>(index)] = Effekseer::Effect::Create(effekseerManager, (EFK_CHAR*)utf16Filename);
}

// 再生
Effekseer::Handle Effect::Play(EffectType index, const DirectX::XMFLOAT3& position, DirectX::XMFLOAT3 angle, float scale)
{
	int effectIndex = static_cast<int>(index);

	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

	Effekseer::Handle handle = effekseerManager->Play(effekseerEffects[effectIndex], position.x, position.y, position.z);
	effekseerManager->SetScale(handle, scale, scale, scale);
	DirectX::XMFLOAT3 radians = {
		DirectX::XMConvertToRadians(angle.x),
		DirectX::XMConvertToRadians(angle.y),
		DirectX::XMConvertToRadians(angle.z),
	};
	effekseerManager->SetRotation(handle, radians.x, radians.y, radians.z);
	return handle;
}

// 停止
void Effect::Stop(Effekseer::Handle handle)
{
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

	effekseerManager->StopEffect(handle);
}

// 座標設定
void Effect::SetPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3& position)
{
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

	effekseerManager->SetLocation(handle, position.x, position.y, position.z);
}

// スケール設定
void Effect::SetScale(Effekseer::Handle handle, const DirectX::XMFLOAT3& scale)
{
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

	effekseerManager->SetScale(handle, scale.x, scale.y, scale.z);
}

// 角度設定
void Effect::SetAngle(Effekseer::Handle handle, const DirectX::XMFLOAT3& angle)
{
	Effekseer::ManagerRef effekseerManager = EffectManager::Instance().GetEffekseerManager();

	effekseerManager->SetRotation(handle, angle.x, angle.y, angle.z);
}

