#pragma once

#include <raylib.h>
#include "types.h"
#include "vec.h"

template<typename T>
struct rect {
    union {
        T data[4];
        struct {
            T x, y, w, h;
        };
    };

    rect() {
        x = y = w = h = 0;
    }

    static rect zero() {
        return rect();
    }

    template<typename T2>
    rect(const rect<T2> &other) {
        x = static_cast<T>(other.x);
        y = static_cast<T>(other.y);
        w = static_cast<T>(other.w);
        h = static_cast<T>(other.h);
    }

    rect(const T &_x, const T &_y, const T &_w, const T &_h) {
        x = _x;
        y = _y;
        w = _w;
        h = _h;
    }

    rect(const vec2<T> &pos, const vec2<T> &size) {
        x = pos.x;
        y = pos.y;
        w = size.x;
        h = size.y;
    }

    operator Rectangle() {
        return {
            static_cast<f32>(x),
            static_cast<f32>(y),
            static_cast<f32>(w),
            static_cast<f32>(h),
        };
    }

    void setPosition(const vec2<T> &pos) {
        x = pos.x;
        y = pos.y;
    }

    void setSize(const vec2<T> &size) {
        w = size.x;
        h = size.y;
    }

    vec2<T> position() const {
        return { x, y };
    }

    vec2<T> size() const {
        return { w, h };
    }

    void scale(const T &s) {
        x *= s;
        y *= s;
        w *= s;
        h *= s;
    }

    void scale(const T &sx, const T &sy) {
        x *= sx;
        y *= sy;
        w *= sx;
        h *= sy;
    }

    void inflate(const T &amount) {
        x -= amount;
        y -= amount;
        w += amount * 2;
        h += amount * 2;
    }

    vec2<T> center() const {
        return { x + (w / 2), y + (h / 2) };
    }

    vec2<T> localCenter() const {
        return { w / 2, h / 2 };
    }

    vec2<T> topLeft() const {
        return { x, y };
    }

    vec2<T> topRight() const {
        return { x + w, y };
    }

    vec2<T> bottomLeft() const {
        return { x, y + h };
    }

    vec2<T> bottomRight() const {
        return { x + w, y + h };
    }

    vec2<T> toLocal(const vec2<T> &point) const {
        return { point.x - x, point.y - y };
    }

    bool contains(const vec2<T> &point) const {
        return (
                point.x >= x && point.x < x + w &&
                point.y >= y && point.y < y + h
        );
    }

    bool intersects(const rect &other) const {
        return (
            (x + w) > other.x &&
            (y + h) > other.y &&
            x < (other.x + other.w) &&
            y < (other.y + other.h)
        );
    }

    bool operator==(const rect &other) const {
        return (
            x == other.x && y == other.y &&
            w == other.w && h == other.h
        );
    }
    
    bool operator!=(const rect &other) const {
        return (
            x != other.x || y != other.y ||
            w != other.w || h != other.h
        );
    }

    rect operator+(const vec2<T> &other) const {
        return {
            x + other.x, y + other.y, w, h
        };
    }

    rect operator-(const vec2<T> &other) const {
        return {
            x - other.x, y - other.y, w, h
        };
    }

    rect &operator+=(const vec2<T> &other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    rect &operator-=(const vec2<T> &other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }
};

typedef rect<u32> rectu;
typedef rect<i32> recti;
typedef rect<f32> rectf;
typedef rect<f64> rectd;