#pragma once 

enum EasingKind
{
	EaseInSine,
	EaseOutSine,
	EaseInOutSine,
	EaseInQuad,
	EaseOutQuad,
	EaseInOutQuad,
	EaseInCubic,
	EaseOutCubic,
	EaseInOutCubic,
	EaseInQuart,
	EaseOutQuart,
	EaseInOutQuart,
	EaseInQuint,
	EaseOutQuint,
	EaseInOutQuint,
	EaseInExpo,
	EaseOutExpo,
	EaseInOutExpo,
	EaseInCirc,
	EaseOutCirc,
	EaseInOutCirc,
	EaseInBack,
	EaseOutBack,
	EaseInOutBack,
	EaseInElastic,
	EaseOutElastic,
	EaseInOutElastic,
	EaseInBounce,
	EaseOutBounce,
	EaseInOutBounce,
	EaseEnd,
};

float Easing(float time, float totalTime, EasingKind kind);

typedef float(*EasingFunction)(float);

EasingFunction easingFunctions[EaseEnd] =
{
	&EaseInSine,
	&EaseOutSine,
	&EaseInOutSine,
	&EaseInQuad,
	&EaseOutQuad,
	&EaseInOutQuad,
	&EaseInCubic,
	&EaseInOutCubic,
	&EaseInQuart,
	&EaseOutQuart,
	&EaseInOutQuart,
	&EaseInQuint,
	&EaseOutQuint,
	&EaseInOutQuint,
	&EaseInExpo,
	&EaseOutExpo,
	&EaseInOutExpo,
	&EaseInCirc,
	&EaseOutCirc,
	&EaseInOutCirc,
	&EaseInBack,
	&EaseOutBack,
	&EaseInOutBack,
	&EaseInElastic,
	&EaseOutElastic,
	&EaseInOutElastic,
	&EaseInBounce,
	&EaseOutBounce,
	&EaseInOutBounce,
};



float EaseInSine(float t);
float EaseOutSine(float t);
float EaseInOutSine(float t);
float EaseInQuad(float t);
float EaseOutQuad(float t);
float EaseInOutQuad(float t);
float EaseInCubic(float t);
float EaseOutCubic(float t);
float EaseInOutCubic(float t);
float EaseInQuart(float t);
float EaseOutQuart(float t);
float EaseInOutQuart(float t);
float EaseInQuint(float t);
float EaseOutQuint(float t);
float EaseInOutQuint(float t);
float EaseInExpo(float t);
float EaseOutExpo(float t);
float EaseInOutExpo(float t);
float EaseInCirc(float t);
float EaseOutCirc(float t);
float EaseInOutCirc(float t);
float EaseInBack(float t);
float EaseOutBack(float t);
float EaseInOutBack(float t);
float EaseInElastic(float t);
float EaseOutElastic(float t);
float EaseInOutElastic(float t);
float EaseInBounce(float t);
float EaseOutBounce(float t);
float EaseInOutBounce(float t);
