#include "wait.h"

#include <stdint.h>

#ifdef _WIN32

Timer::Timer() {
    timeBeginPeriod(1);
    QueryPerformanceFrequency(&high_perf_timer_freq);    
}

Timer::~Timer() {
    timeEndPeriod(1);
}

// from http://www.geisswerks.com/ryan/FAQS/timing.html
void Timer::wait(uint32_t microseconds) {
    LARGE_INTEGER start;
    QueryPerformanceCounter(&start);

    int ticks_to_wait = (int)high_perf_timer_freq.QuadPart / 1'000'000 * microseconds;
    bool done = false;
    LARGE_INTEGER now;
    
    do {
        QueryPerformanceCounter(&now);
        
        int ticks_passed = (int)((int64_t)now.QuadPart - (int64_t)start.QuadPart);
        int ticks_left = ticks_to_wait - ticks_passed;

        if (now.QuadPart < start.QuadPart)    // time wrap
            done = true;
        if (ticks_passed >= ticks_to_wait)
            done = true;
        
        if (!done) {
            // if > 0.002s left, do Sleep(1), which will actually sleep some 
            //   steady amount, probably 1-2 ms,
            //   and do so in a nice way (cpu meter drops; laptop battery spared).
            // otherwise, do a few Sleep(0)'s, which just give up the timeslice,
            //   but don't really save cpu or battery, but do pass a tiny
            //   amount of time.
            if (ticks_left > (int)high_perf_timer_freq.QuadPart*2/1000) {
                Sleep(1);
            }
            else {
                for (int i = 0; i < 10; i++) {
                    Sleep(0);  // causes thread to give up its timeslice
                }
            }                        
        }
    } while (!done);    
}

#else

#include <time.h>

Timer::Timer() {}
Timer::~Timer() {}

void Timer::wait(uint32_t microseconds) {
    timespec t;
    t.tv_sec = microseconds / 1'000'000;
    t.tv_nsec = (microseconds - (t.tv_sec * 1'000'000)) * 1000;
    nanosleep(&t, NULL);
}

#endif