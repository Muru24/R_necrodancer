#include "Timer.h"

// 현재 시간을 측정하여 이전 프레임 이후 흐른 시간(Delta Time)을 계산합니다.
float Timer::GetDeltaTime()
{
    LARGE_INTEGER currentTime;
    QueryPerformanceCounter(&currentTime); 

    // 경과한 틱 수를 주파수로 나누어 초 단위 시간 계산
    float deltaTime = static_cast<float>(currentTime.QuadPart - lastTime.QuadPart) / frequency.QuadPart;

    // 현재 시간을 마지막 측정 시간으로 업데이트
    lastTime = currentTime;

    return deltaTime;
}
