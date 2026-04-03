#pragma once
#include <Windows.h>

// 고해상도 타이머를 사용하여 프레임 간 경과 시간(Delta Time)을 계산하는 클래스
class Timer
{
private:
    LARGE_INTEGER frequency; // 초당 틱 횟수
    LARGE_INTEGER lastTime;  // 마지막으로 기록된 틱 값

public:
    Timer() {
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&lastTime);
    }

    // 이전 호출 시점부터 현재까지의 경과 시간을 초 단위로 반환합니다.
    float GetDeltaTime();
};
