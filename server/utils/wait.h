#pragma once

#include <stdint.h>

#ifdef _WIN32
#define VC_EXTRALEAN
#include <windows.h>
#endif

class Timer {
public:
    Timer();
    ~Timer();

    void wait(uint32_t microseconds);

#ifdef _WIN32
private:
    LARGE_INTEGER high_perf_timer_freq;
#endif
};