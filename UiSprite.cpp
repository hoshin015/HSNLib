#include "UiSprite.h"
#include "Library/Easing.h"
#include "Library/Timer.h"
#include <iostream>
#include <iomanip>
#include <sstream>

#define ResultUiTextEasing 1

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

void UiSprite::UiTextUpdate()
{

#if ResultUiTextEasing
	float score = targetScore * Easing(timer, easingTime, easeCirc, easeIn);
#else
	startScore++;
	if (startScore > targetScore) startScore = targetScore;
	float score = startScore;
#endif
	
	text = std::to_string(static_cast<int>(score));

	timer += (showUi) ? Timer::Instance().DeltaTime() : -Timer::Instance().DeltaTime();

	if (timer > easingTime) timer = easingTime;
	if (timer < 0.0f) timer = 0.0f;
}

void UiSprite::UiTextRender()
{
	sprite->SprTextOut(text, pos, { 50,50 });
}

void UiSprite::UiTextTimeUpdate()
{

#if ResultUiTextEasing
	float score = targetScore * Easing(timer, easingTime, easeCirc, easeIn);
#else
	startScore++;
	if (startScore > targetScore) startScore = targetScore;
	float score = startScore;
#endif


	int min = static_cast<int>(score) / 60;
	int seconds = static_cast<int>(score) % 60;

	std::stringstream ss;
	ss << std::setw(2) << std::setfill('0') << min << ":" << std::setw(2) << std::setfill('0') << seconds;

	text = ss.str();

	timer += (showUi) ? Timer::Instance().DeltaTime() : -Timer::Instance().DeltaTime();

	if (timer > easingTime) timer = easingTime;
	if (timer < 0.0f) timer = 0.0f;
}