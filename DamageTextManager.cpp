#include "DamageTextManager.h"

// 更新処理
void DamageTextManager::Update()
{
	// 更新処理
	for (DamageText* damageText : damageTexts)
	{
		damageText->Update();
	}

	// 破棄処理
	for (DamageText* damageText : removes)
	{
		std::vector<DamageText*>::iterator it = std::find(damageTexts.begin(), damageTexts.end(), damageText);

		if (it != damageTexts.end())
		{
			damageTexts.erase(it);
		}
		// ダメージテキストの破棄処理
		delete damageText;
	}
	// 破棄リストをクリア
	removes.clear();
}

//　描画処理
void DamageTextManager::Render()
{
	for (DamageText* damageText : damageTexts)
	{
		damageText->Render();
	}
}

// ダメージテキスト登録
void DamageTextManager::Register(DamageText* damageText)
{
	damageTexts.emplace_back(damageText);
}


// ダメージテキスト削除
void DamageTextManager::Remove(DamageText* damageText)
{
	// 破棄リストに追加
	removes.insert(damageText);
}

// ダメージテキスト全削除
void DamageTextManager::Clear()
{
	for (DamageText* damageText : damageTexts)
	{
		delete damageText;
	}
	damageTexts.clear();
}
