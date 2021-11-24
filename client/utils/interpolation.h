#pragma once

#include <math.h>
#include "types.h"

#ifndef PI
    #define PI 3.14159265358979323846f
#endif

// from https://easings.net/#easeOutElastic
inline f32 easeOutElastic(f32 t) {
    static constexpr f32 c4 = (PI * 2.f) / 3.f;

    return t <= 0 ? 0 :
           t >= 1 ? 1 :
           pow(2, -10 * t) * sin((t * 10 - 0.75) * c4) + 1;
}

inline f32 linear(f32 t) {
    return t;
}

template<f32 (*fun)(f32), typename T>
T lerp(T from, T to, f32 t) {
    return (to - from) * fun(clamp(t, 0.f, 1.f)) + from;
}