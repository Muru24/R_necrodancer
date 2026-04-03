#pragma once
#include "Struct.h"

// 오브젝트 간의 충돌 검사를 처리하는 클래스
class Collider
{
	// 대상 오브젝트와의 충돌 여부를 확인합니다.
	// - collider: 충돌 검사 대상 오브젝트
	bool CheckCollider(Object collider);
};
