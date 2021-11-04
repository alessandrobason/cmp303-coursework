#pragma once

#include <vector>
#include <raylib.h>
#include "types.h"

template<typename T>
class CachedVec {
public:
    CachedVec(f32 expiration_time = 3.f)
        : expiration_time(expiration_time) {}

    T &push(const T &to_push) {
        for(auto &val: vec) {
            if(val == to_push) {
                return val;
            }
        }
        timers.emplace_back(expiration_time);
        return vec.emplace_back(to_push);
    }

    T &push(const T &&to_push) {
        for(auto &val: vec) {
            if(val == to_push) {
                return val;
            }
        }
        timers.emplace_back(expiration_time);
        return vec.emplace_back(to_push);
    }

    void update(f32 deltatime) {
        for(size_t i = 0; i < timers.size(); ++i) {
            timers[i] -= deltatime;
            if(timers[i] <= 0.f) {
                removeAt(i);
                --i;
            }
        }
    }

    void removeAt(size_t index) {
        vec[index] = std::move(vec.back());
        timers[index] = std::move(timers.back());

        vec.pop_back();
        timers.pop_back();
    }

    size_t size() const {
        return vec.size();
    }

    f32 getTimeLeft(size_t index) const {
        return timers[index];
    }

    void resetTimer(size_t index) {
        timers[index] = expiration_time;
    }

    T &operator[](size_t index) {
        return vec[index];
    }

    const T &operator[](size_t index) const {
        return vec[index];
    }

    T *begin() {
        return vec.data();
    }

    T *end() {
        return vec.data() + vec.size();
    }

    const T *begin() const {
        return vec.data();
    }

    const T *end() const {
        return vec.data() + vec.size();
    }


private:
    f32 expiration_time;
    std::vector<T> vec;
    std::vector<f32> timers;
};