// #include <raylib.h>
#include <tracelog.h>

#include <dirwatch.h>
#include <cthreads.h>
#include <tracelog.h>
#include <windows.h>

void onEvent(const char *path, int action, dirwatch_file_t data) {
    info("name: %s", data.name);
    if(action == DIRWATCH_FILE_REMOVED) thrExit(0);
}

int value1 = 0;
int value2 = 0;
cmutex_t mtx;

int thread1(void *args) {
    mtxLock(mtx);
    Sleep(500);
    value1 += 5;
    value2 = value1 * 2;
    mtxUnlock(mtx);
    return 0;
}

int thread2(void *args) {
    mtxLock(mtx);
    value1 += 15 + value2;
    value2 -= value1;
    mtxUnlock(mtx);
    return 0;
}

int main() {
    mtx = mtxInit();

    cthread_t t1 = thrCreate(thread1, NULL);
    cthread_t t2 = thrCreate(thread2, NULL);

    thrJoin(t1, NULL);
    thrJoin(t2, NULL);

    info("value1: %d", value1);
    info("value2: %d", value2);

    // dirwatch_desc_t desc;
    // desc.on_event = onEvent;
    // desc.path = ".";
    // dirwatch_t dir = watchDir(desc);
    
    // waitForWatchDir(&dir);
    /*
    InitWindow(500, 500, "online game");

    SetTargetFPS(60);

    while(!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(RAYWHITE);
        EndDrawing();
    }

    CloseWindow();
    */
}