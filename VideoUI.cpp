#include "VideoUI.h"
#include "Library/Graphics/Graphics.h"
#include "Library/Text/DispString.h"
using namespace DirectX;

constexpr float kBlackRatio = 0.01f;
constexpr float kVideoRatio = 0.7f;
constexpr float kTextRatio = 0.3f;

void VideoUI::Draw(DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 size) {
	//黒いとこ
	_rect.Render(
		position.x - size.x * .5f, position.y - size.y * .5f,
		size.x, size.y,
		0, 0, 0, 1,
		0
	);
	float blackLineSize = { size.x * kBlackRatio };

	XMFLOAT2 priVideoPos = { position.x, position.y + size.y * (-0.5f + kVideoRatio * .5f) };
	XMFLOAT2 priVideoSize = { size.x - blackLineSize, size.y * kVideoRatio - blackLineSize };
	//動画のエリア(全体の7割)
	_rect.Render(
		priVideoPos.x - priVideoSize.x * .5f, priVideoPos.y - priVideoSize.y * .5f,
		priVideoSize.x, priVideoSize.y,
		.6f, .6f, .6f, 1,
		0
	);

	XMFLOAT2 priTextPos = { position.x, position.y - size.y * (-0.5f + kTextRatio * .5f) };
	XMFLOAT2 priTextSize = { size.x - blackLineSize, size.y * kTextRatio - blackLineSize };
	//文章のエリア(全体の3割)
	_rect.Render(
		priTextPos.x - priTextSize.x * .5f, priTextPos.y - priTextSize.y * .5f,
		priTextSize.x, priTextSize.y,
		.6f, .6f, .6f, 1,
		0
	);

	Graphics& gfx = Graphics::Instance();
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
