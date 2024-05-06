#pragma once
#include "Video.h"
#include "Library/2D/Primitive2D.h"
#include "Library/2D/Sprite.h"

class VideoUI {
public:
	VideoUI() {}

	void SetVideo(Video* video) { _video = video; }
	void SetTextSprite(Sprite* text) { _text = text; }
	void Draw(DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 size);
private:
	Video* _video;
	Sprite* _text;
	Primitive2D _rect;
};
