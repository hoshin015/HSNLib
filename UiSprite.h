#pragma once
#include <memory>
#include "Library/2D/Sprite.h"

class UiSprite
{
public:
	UiSprite(const wchar_t* path);
	~UiSprite() {};

	DirectX::XMFLOAT2 pos = {};
	DirectX::XMFLOAT2 size = { 100,100 };
	DirectX::XMFLOAT4 color = { 1,1,1,1 };
	float angle = 0.0f;

	// �C�[�W���O�p
	bool showUi = false;
	float timer = 0.0f;
	float easingTime = 0.3f;
	
	// �ʏ�\���p
	DirectX::XMFLOAT2 hidePosition = {};
	DirectX::XMFLOAT2 showPosition = {};

	void UiUpdate();
	void UiRender();

	// �X�R�A�\���p
	float startScore = 0;
	float targetScore = 0;
	std::string text = "";
	void UiTextUpdate();
	void UiTextRender();
	void UiTextTimeUpdate();


	std::unique_ptr<Sprite> sprite;
};