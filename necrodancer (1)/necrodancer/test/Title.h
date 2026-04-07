#pragma once
#include <windows.h>
#include <gdiplus.h>
#include <math.h>
#include "Define.h"

class Title
{
private:
	int m_nSelectedIdx;
	float m_fAnimTimer;
	bool m_bIsVisible;

public:
	Title();
	~Title();

	void Initialize();
	void Update();
	void tRender(Gdiplus::Graphics& graphics);

	int GetSelectedIdx() const { return m_nSelectedIdx; }
};
