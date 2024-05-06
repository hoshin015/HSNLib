#include "UiSprite.h"
#include "Library/Easing.h"
#include "Library/Timer.h"

UiSprite::UiSprite(const wchar_t* path)
{
	sprite = std::make_unique<Sprite>(path);
}

void UiSprite::UiUpdate()
{
	pos.x = hidePosition.x + (showPosition.x - hidePosition.x) * Easing(timer, easingTime, easeQuart, easeOut);
	pos.y = hidePosition.y + (showPosition.y - hidePosition.y) * Easing(timer, easingTime, easeQuart, easeOut);

	timer += (showUi) ? Timer::Instance().DeltaTime() : -Timer::Instance().DeltaTime();
	
	if (timer > easingTime) timer = easingTime;
	if (timer < 0.0f) timer = 0.0f;
}

void UiSprite::UiRender()
{
	sprite->Render(pos.x, pos.y, size.x, size.y, color.x, color.y, color.z, color.w, angle);
}
