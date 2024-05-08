#pragma once
#include "Video.h"
#include "Library/2D/Primitive2D.h"
#include "Library/2D/Sprite.h"

class VideoUI {
public:
	VideoUI() {}

	void Draw(DirectX::XMFLOAT4 color = { 1,1,1,1 }, bool drawBG = false);

	void SetVideo(Video* video) { _video = video; }
	void SetTextSprite(Sprite* text) { _text = text; }
	void SetPosition(DirectX::XMFLOAT2 pos) { _pos = pos; }
	void SetSize(DirectX::XMFLOAT2 size) { _size = size; }

	Video* GetVideo() { return _video; }
private:
	Video* _video;
	Sprite* _text;
	Primitive2D _rect;

	DirectX::XMFLOAT2 _pos;
	DirectX::XMFLOAT2 _size;
};
