#pragma once

template<typename T>
T max(const T &a, const T &b) {
    return a > b ? a : b;
}

template<typename T>
T min(const T &a, const T &b) {
    return a < b ? a : b;
}

template<typename T>
T clamp(const T &val, const T &min_val, const T &max_val) {
    return min(max(val, min_val), max_val);
}

template<typename T>
T pow2(const T &val) {
    return val * val;
}