#include <stdio.h>
#include <stdint.h>
#include <tracelog.h>

#include <utils/wait.h>

#define SEC_TO_MICRO 1'000'000
#define SEC_TO_MILLI 1'000
#define MILLI_TO_MICRO 1'000

int main() {
    Timer timer;
    info("waiting for 500ms 10 times");
    for(int i = 0; i < 10; ++i) {
        timer.wait(1050 * MILLI_TO_MICRO);
        info("waited %d times", i + 1);
    }
    info("finished");
}