#pragma once
#include <Windows.h>

class Timer
{
private:
    LARGE_INTEGER frequency;
    LARGE_INTEGER lastTime;

public:
    static Timer& GetInstance()
    {
        static Timer instance;
        return instance;
    }
    Timer() {
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&lastTime);
    }

    float GetDeltaTime();
};
