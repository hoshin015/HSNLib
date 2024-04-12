#include <cmath>
#include <map>

#include "Easing.h"

EasingFunction easingFunctions[easeEnd] =
{
    &EaseSine,
    &EaseQuad,
    &EaseCubic,
    &EaseQuart,
    &EaseQuint,
    &EaseExpo,
    &EaseCirc,
    &EaseBack,
    &EaseElastic,
    &EaseBounce,
};


float Easing(float time, float totalTime, EasingKind kind, EasingOption option)
{
    float t = time / totalTime;
    if (t > 1) t = 1;
    if (t < 0) t = 0;
    return easingFunctions[kind](t, option);
}



float EaseSine(float t, EasingOption option) {
    // In
    if (option == EasingOption::easeIn)
    {
        return sin(1.5707963 * t);
    }
    // Out
    else
    {
        return 1 + sin(1.5707963 * (--t));
    }
}

float EaseQuad(float t, EasingOption option) {
    // In
    if (option == EasingOption::easeIn)
    {
        return t * t;
    }
    // Out
    else
    {
        return t * (2 - t);
    }
}

float EaseCubic(float t, EasingOption option) {
    // In
    if (option == EasingOption::easeIn)
    {
        return t * t * t;
    }
    // Out
    else
    {
        return 1 + (--t) * t * t;
    }
}

float EaseQuart(float t, EasingOption option) {
    // In
    if (option == EasingOption::easeIn)
    {
        t *= t;
        return t * t;
    }
    // Out
    else
    {
        t = (--t) * t;
        return 1 - t * t;
    }
}

float EaseQuint(float t, EasingOption option) {
    // In
    if (option == EasingOption::easeIn)
    {
        float t2 = t * t;
        return t * t2 * t2;
    }
    // Out
    else
    {
        float t2 = (--t) * t;
        return 1 + t * t2 * t2;
    }   
}

float EaseExpo(float t, EasingOption option) {
    // In
    if (option == EasingOption::easeIn)
    {
        return (pow(2, 8 * t) - 1) / 255;
    }
    // Out
    else
    {
        return 1 - pow(2, -8 * t);
    }
}

float EaseCirc(float t, EasingOption option) {
    // In
    if (option == EasingOption::easeIn)
    {
        return 1 - sqrt(1 - t);
    }
    // Out
    else
    {
        return sqrt(t);
    }
}


float EaseBack(float t, EasingOption option) {
    // In
    if (option == EasingOption::easeIn)
    {
        return t * t * (2.70158 * t - 1.70158);
    }
    // Out
    else
    {
        return 1 + (--t) * t * (2.70158 * t + 1.70158);
    }
}

float EaseElastic(float t, EasingOption option) {
    // In
    if (option == EasingOption::easeIn)
    {
        float t2 = t * t;
        return t2 * t2 * sin(t * PI * 4.5);
    }
    // Out
    else
    {
        float t2 = (t - 1) * (t - 1);
        return 1 - t2 * t2 * cos(t * PI * 4.5);
    }
}

float EaseBounce(float t, EasingOption option) {
    // In
    if (option == EasingOption::easeIn)
    {
        return pow(2, 6 * (t - 1)) * abs(sin(t * PI * 3.5));
    }
    // Out
    else
    {
        return 1 - pow(2, -6 * t) * abs(cos(t * PI * 3.5));
    }
}
