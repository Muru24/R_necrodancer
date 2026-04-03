#pragma once
#include <Windows.h>

class Timer
{
private:
    LARGE_INTEGER frequency;
    LARGE_INTEGER lastTime;

public:
    Timer() {
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&lastTime);
    }

    float GetDeltaTime();
};
