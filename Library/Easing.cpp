#include <cmath>
#include <map>

#include "Easing.h"

EasingFunction easingFunctions[easeEnd] =
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


float Easing(float time, float totalTime, EasingKind kind, EasingOption option)
{
    float t = time / totalTime;
    if (t > 1) t = 1;
    if (t < 0) t = 0;
    return easingFunctions[kind](t, option);
}



float EaseInSine(float t, EasingOption option) {
    return sin(1.5707963 * t);
}

float EaseOutSine(float t, EasingOption option) {
    return 1 + sin(1.5707963 * (--t));
}

float EaseInOutSine(float t, EasingOption option) {
    return 0.5 * (1 + sin(3.1415926 * (t - 0.5)));
}

float EaseInQuad(float t, EasingOption option) {
    return t * t;
}

float EaseOutQuad(float t, EasingOption option) {
    return t * (2 - t);
}

float EaseInOutQuad(float t, EasingOption option) {
    return t < 0.5 ? 2 * t * t : t * (4 - 2 * t) - 1;
}

float EaseInCubic(float t, EasingOption option) {
    return t * t * t;
}

float EaseOutCubic(float t, EasingOption option) {
    return 1 + (--t) * t * t;
}

float EaseInOutCubic(float t, EasingOption option) {
    return t < 0.5 ? 4 * t * t * t : 1 + (--t) * (2 * (--t)) * (2 * t);
}

float EaseInQuart(float t, EasingOption option) {
    t *= t;
    return t * t;
}

float EaseOutQuart(float t, EasingOption option) {
    t = (--t) * t;
    return 1 - t * t;
}

float EaseInOutQuart(float t, EasingOption option) {
    if (t < 0.5) {
        t *= t;
        return 8 * t * t;
    }
    else {
        t = (--t) * t;
        return 1 - 8 * t * t;
    }
}

float EaseInQuint(float t, EasingOption option) {
    float t2 = t * t;
    return t * t2 * t2;
}

float EaseOutQuint(float t, EasingOption option) {
    float t2 = (--t) * t;
    return 1 + t * t2 * t2;
}

float EaseInOutQuint(float t, EasingOption option) {
    float t2;
    if (t < 0.5) {
        t2 = t * t;
        return 16 * t * t2 * t2;
    }
    else {
        t2 = (--t) * t;
        return 1 + 16 * t * t2 * t2;
    }
}

float EaseInExpo(float t, EasingOption option) {
    return (pow(2, 8 * t) - 1) / 255;
}

float EaseOutExpo(float t, EasingOption option) {
    return 1 - pow(2, -8 * t);
}

float EaseInOutExpo(float t, EasingOption option) {
    if (t < 0.5) {
        return (pow(2, 16 * t) - 1) / 510;
    }
    else {
        return 1 - 0.5 * pow(2, -16 * (t - 0.5));
    }
}

float EaseInCirc(float t, EasingOption option) {
    return 1 - sqrt(1 - t);
}

float EaseOutCirc(float t, EasingOption option) {
    return sqrt(t);
}

float EaseInOutCirc(float t, EasingOption option) {
    if (t < 0.5) {
        return (1 - sqrt(1 - 2 * t)) * 0.5;
    }
    else {
        return (1 + sqrt(2 * t - 1)) * 0.5;
    }
}

float EaseInBack(float t, EasingOption option) {
    return t * t * (2.70158 * t - 1.70158);
}

float EaseOutBack(float t, EasingOption option) {
    return 1 + (--t) * t * (2.70158 * t + 1.70158);
}

float EaseInOutBack(float t, EasingOption option) {
    if (t < 0.5) {
        return t * t * (7 * t - 2.5) * 2;
    }
    else {
        return 1 + (--t) * t * 2 * (7 * t + 2.5);
    }
}

float EaseInElastic(float t, EasingOption option) {
    float t2 = t * t;
    return t2 * t2 * sin(t * PI * 4.5);
}

float EaseOutElastic(float t, EasingOption option) {
    float t2 = (t - 1) * (t - 1);
    return 1 - t2 * t2 * cos(t * PI * 4.5);
}

float EaseInOutElastic(float t, EasingOption option) {
    float t2;
    if (t < 0.45) {
        t2 = t * t;
        return 8 * t2 * t2 * sin(t * PI * 9);
    }
    else if (t < 0.55) {
        return 0.5 + 0.75 * sin(t * PI * 4);
    }
    else {
        t2 = (t - 1) * (t - 1);
        return 1 - 8 * t2 * t2 * sin(t * PI * 9);
    }
}

float EaseInBounce(float t, EasingOption option) {
    return pow(2, 6 * (t - 1)) * abs(sin(t * PI * 3.5));
}

float EaseOutBounce(float t, EasingOption option) {
    return 1 - pow(2, -6 * t) * abs(cos(t * PI * 3.5));
}

float EaseInOutBounce(float t, EasingOption option) {
    if (t < 0.5) {
        return 8 * pow(2, 8 * (t - 1)) * abs(sin(t * PI * 7));
    }
    else {
        return 1 - 8 * pow(2, -8 * t) * abs(sin(t * PI * 7));
    }
}
