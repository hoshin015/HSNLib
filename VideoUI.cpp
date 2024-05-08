#include "VideoUI.h"
#include "Library/Graphics/Graphics.h"
#include "Library/Framework.h"
#include "Library/Text/DispString.h"
using namespace DirectX;

constexpr float kBlackRatio = 0.01f;
constexpr float kVideoRatio = 0.7f;
constexpr float kTextRatio = 0.3f;

void VideoUI::Draw(DirectX::XMFLOAT4 color, bool drawBG) {
	Graphics& gfx = Graphics::Instance();
	float wWidth = Framework::Instance().windowWidth;
	float wHeight = Framework::Instance().windowHeight;
	//背景
	if (drawBG)_rect.Render(0, 0, wWidth, wHeight, 0, 0, 0, 0.5f, 0);

	//黒いとこ
	_rect.Render(
		_pos.x - _size.x * .5f, _pos.y - _size.y * .5f,
		_size.x, _size.y,
		0, 0, 0, 1,
		0
	);
	float blackLineSize = { _size.x * kBlackRatio };

	XMFLOAT2 priVideoPos = { _pos.x, _pos.y + _size.y * (-0.5f + kVideoRatio * .5f) };
	XMFLOAT2 priVideoSize = { _size.x - blackLineSize, _size.y * kVideoRatio - blackLineSize };
	//動画のエリア(全体の7割)
	_rect.Render(
		priVideoPos.x - priVideoSize.x * .5f, priVideoPos.y - priVideoSize.y * .5f,
		priVideoSize.x, priVideoSize.y,
		color.x, color.y, color.z, color.w,
		0
	);

	XMFLOAT2 priTextPos = { _pos.x, _pos.y - _size.y * (-0.5f + kTextRatio * .5f) };
	XMFLOAT2 priTextSize = { _size.x - blackLineSize, _size.y * kTextRatio - blackLineSize };
	//文章のエリア(全体の3割)
	_rect.Render(
		priTextPos.x - priTextSize.x * .5f, priTextPos.y - priTextSize.y * .5f,
		priTextSize.x, priTextSize.y,
		color.x, color.y, color.z, color.w,
		0
	);

	XMFLOAT2 sourceVideoSize = _video->GetSourceSize();
	XMFLOAT2 videoSize;
	float asp = sourceVideoSize.x / sourceVideoSize.y;
	if (priVideoSize.x > (priVideoSize.y - blackLineSize) * asp) {
		videoSize = { (priVideoSize.y - blackLineSize) * asp , (priVideoSize.y - blackLineSize) };
	}
	else {
		asp = sourceVideoSize.y / sourceVideoSize.x;
		videoSize = { (priVideoSize.x - blackLineSize) , (priVideoSize.x - blackLineSize) * asp };
	}
	_video->Render(
		gfx.deviceContext.Get(),
		{ priVideoPos.x - videoSize.x * .5f, priVideoPos.y - videoSize.y * .5f },
		{ videoSize.x, videoSize.y }
	);

	XMFLOAT2 sourceTextSize = _text->GetTexSize();
	XMFLOAT2 textSize;
	asp = sourceTextSize.x / sourceTextSize.y;
	if (priTextSize.x > (priTextSize.y - blackLineSize) * asp) {
		textSize = { (priTextSize.y - blackLineSize) * asp , (priTextSize.y - blackLineSize) };
	}
	else {
		asp = sourceTextSize.y / sourceTextSize.x;
		textSize = { (priTextSize.x - blackLineSize) , (priTextSize.x - blackLineSize) * asp };
	}
	_text->Render(
		priTextPos.x - textSize.x * .5f, priTextPos.y - textSize.y * .5f,
		textSize.x, textSize.y,
		1, 1, 1, 1,
		0
	);

	//DispString::Instance().Draw(str, priTextPos, 30, TEXT_ALIGN::MIDDLE, {0,0,0,1});
}
