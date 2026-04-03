#include "Timer.h"

float Timer::GetDeltaTime()
{
    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime); 

    float deltaTime = static_cast<float>(currentTime.QuadPart - lastTime.QuadPart) / frequency.QuadPart;

    lastTime = currentTime;

    return deltaTime;
}
