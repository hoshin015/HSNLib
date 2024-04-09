#pragma once

#include <vector>
#include <set>
#include "DamageText.h"

// ダメージテキストマネージャー
class DamageTextManager
{
private:
	DamageTextManager() {}
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

private:
	std::vector<DamageText*> damageTexts;
	std::set<DamageText*> removes;
};