#pragma once
#include "Define.h"

class R_Note
{
private:
	float cTime;
	float mTime;
	float p_checkTime;
	float m_checkTime;

	bool isAlive;
public:
	R_Note() {}
	R_Note(float _bpm) {
		cTime = 0;
		mTime = 60.0f / _bpm; 
		p_checkTime = RHYTHM_MARGIN; 
		m_checkTime = mTime - RHYTHM_MARGIN;
		isAlive = false;
	}
	~R_Note() { }

	bool GetIsAlive()const { return isAlive; }
	float Getcurrenttime() const { return cTime; }
	float Getmaxtime() const { return mTime; }

	bool OnNoteInputCompleted();
	void UpdateNoteTime(bool& outBeatEvent);

	void StartNote();
};

