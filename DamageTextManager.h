#pragma once

#include <memory>
#include <vector>
#include <set>
#include "DamageText.h"
#include "Library/2D/Sprite.h"

// ダメージテキストマネージャー
class DamageTextManager
{
private:
	DamageTextManager() { textSpr = std::make_unique<Sprite>(L"Data/Font/fontDamage.png"); }
	~DamageTextManager() {}

public:
	// 唯一のインスタンス取得
	static DamageTextManager& Instance()
	{
		static DamageTextManager instance;
		return instance;
	}

	// 更新処理
	void Update();

	//　描画処理
	void Render();

	// ダメージテキスト登録
	void Register(DamageText* damageText);

	// ダメージテキスト削除
	void Remove(DamageText* damageText);

	// ダメージテキスト全削除
	void Clear();

	// ダメージテキスト数取得
	int GetDamageTextCount() const { return static_cast<int>(damageTexts.size()); }

	// ダメージテキスト取得
	DamageText* GetDamageText(int index) { return damageTexts.at(index); }

	// デバッグ用GUI描画
	void DrawDebugGui();

	std::unique_ptr<Sprite> textSpr;
private:
	std::vector<DamageText*> damageTexts;
	std::set<DamageText*> removes;


};