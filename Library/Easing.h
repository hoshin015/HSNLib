#pragma once 
#define PI 3.1415926545

enum EasingKind
{
	easeSine,
	easeQuad,
	easeCubic,
	easeQuart,
	easeQuint,
	easeExpo,
	easeCirc,
	easeBack,
	easeElastic,
	easeBounce,
	easeEnd,
};

enum EasingOption
{
	easeIn,		// 0
	easeOut,	// 1
};


float EaseSine(float t, EasingOption option);
float EaseQuad(float t, EasingOption option);
float EaseCubic(float t, EasingOption option);
float EaseQuart(float t, EasingOption option);
float EaseQuint(float t, EasingOption option);
float EaseExpo(float t, EasingOption option);
float EaseCirc(float t, EasingOption option);
float EaseBack(float t, EasingOption option);
float EaseElastic(float t, EasingOption option);
float EaseBounce(float t, EasingOption option);

float Easing(float time, float totalTime, EasingKind kind, EasingOption option);

typedef float(*EasingFunction)(float, EasingOption);

extern EasingFunction easingFunctions[easeEnd];


