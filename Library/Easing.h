#pragma once 
#define PI 3.1415926545

enum EasingKind
{
	easeInSine,
	easeOutSine,
	easeInOutSine,
	easeInQuad,
	easeOutQuad,
	easeInOutQuad,
	easeInCubic,
	easeOutCubic,
	easeInOutCubic,
	easeInQuart,
	easeOutQuart,
	easeInOutQuart,
	easeInQuint,
	easeOutQuint,
	easeInOutQuint,
	easeInExpo,
	easeOutExpo,
	easeInOutExpo,
	easeInCirc,
	easeOutCirc,
	easeInOutCirc,
	easeInBack,
	easeOutBack,
	easeInOutBack,
	easeInElastic,
	easeOutElastic,
	easeInOutElastic,
	easeInBounce,
	easeOutBounce,
	easeInOutBounce,
	easeEnd,
};

enum EasingOption
{
	easeIn,		// 0
	easeOut,	// 1
};


float EaseInSine(float t, EasingOption option);
float EaseOutSine(float t, EasingOption option);
float EaseInOutSine(float t, EasingOption option);
float EaseInQuad(float t, EasingOption option);
float EaseOutQuad(float t, EasingOption option);
float EaseInOutQuad(float t, EasingOption option);
float EaseInCubic(float t, EasingOption option);
float EaseOutCubic(float t, EasingOption option);
float EaseInOutCubic(float t, EasingOption option);
float EaseInQuart(float t, EasingOption option);
float EaseOutQuart(float t, EasingOption option);
float EaseInOutQuart(float t, EasingOption option);
float EaseInQuint(float t, EasingOption option);
float EaseOutQuint(float t, EasingOption option);
float EaseInOutQuint(float t, EasingOption option);
float EaseInExpo(float t, EasingOption option);
float EaseOutExpo(float t, EasingOption option);
float EaseInOutExpo(float t, EasingOption option);
float EaseInCirc(float t, EasingOption option);
float EaseOutCirc(float t, EasingOption option);
float EaseInOutCirc(float t, EasingOption option);
float EaseInBack(float t, EasingOption option);
float EaseOutBack(float t, EasingOption option);
float EaseInOutBack(float t, EasingOption option);
float EaseInElastic(float t, EasingOption option);
float EaseOutElastic(float t, EasingOption option);
float EaseInOutElastic(float t, EasingOption option);
float EaseInBounce(float t, EasingOption option);
float EaseOutBounce(float t, EasingOption option);
float EaseInOutBounce(float t, EasingOption option);

float Easing(float time, float totalTime, EasingKind kind, EasingOption option);

typedef float(*EasingFunction)(float, EasingOption);

extern EasingFunction easingFunctions[easeEnd];


