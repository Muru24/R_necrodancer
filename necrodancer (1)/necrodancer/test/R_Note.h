#pragma once
#include "Define.h"

class R_Note
{
private:
	float cTime;
	float mTime;

	bool isAlive;
public:
	R_Note() {}
	R_Note(float _bpm) {
		cTime = 0;
		mTime = 60.0f / _bpm; 
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

