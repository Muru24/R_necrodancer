#include "R_Note.h"
#include "Timer.h"

bool R_Note::OnNoteInputCompleted()
{
	if (cTime <= p_checkTime || cTime >= m_checkTime)
		return true;

	return false;
}

#include "MainGame.h"

void R_Note::UpdateNoteTime(bool& outBeatEvent)
{
	outBeatEvent = false;
	if (isAlive)
	{
		cTime += MainGame::getInstance().GetDeltaTime();
		if (cTime >= mTime)
		{
			cTime -= mTime; 
			outBeatEvent = true;
		}
	}
}

void R_Note::StartNote()
{
	isAlive = true;
	cTime = 0;
}


