#pragma once

#include <DirectXMath.h>
#include <tchar.h>

class DamageText
{
public:
	DamageText(DirectX::XMFLOAT3 pos, const TCHAR* str)
	{
		position = pos;
		// 文字列の長さ + 1（null 終端文字分）のメモリを動的に確保し、文字列をコピーする
		text = new TCHAR[_tcslen(str) + 1];
		_tcscpy_s(const_cast<TCHAR*>(text), _tcslen(str) + 1, str);
	}
	~DamageText()  {}

	// 更新処理
	void Update();

	// 描画処理
	void Render();

	// 破棄
	void Destroy();

	// 座標更新
	void UpdatePosition();

	//--------------------------------------------------------------
	//  ステート管理
	//--------------------------------------------------------------

	// 待機ステートへ遷移
	void TransitionIdleState();
	// 待機ステート更新処理
	void UpdateIdleState();

	// フェードアウトステートへ遷移
	void TransitionFadeOutState();
	// フェードアウトステート更新処理
	void UpdateFadeOutState();

private:
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 drawPosition = { 0,0 };
	const TCHAR* text;
	float size = 32;

	float timer = 0.0f;
	float idleTime = 0.5f;
	float fadeOutTime = 0.15f;

	float alpha = 1.0f;

	float addY = 0.0f;

	enum class State
	{
		Idle,
		fadeOut,
	};
	State state = State::Idle;
};
