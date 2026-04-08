#pragma once
#include "Timer.h"

class R_Note;

class RhythmManager
{
private:
	R_Note* Note;
	int currentFrame = 0;
	unsigned long lastAnimTime = 0;
	RhythmManager() {}
	~RhythmManager() {}

public:
	static RhythmManager& getInstance() {
		static RhythmManager instance;
		return instance;
	}


	void SetNoteList(float bpm);

	int GetCurrentFrame()const { return currentFrame; }
	unsigned long GetLastAnimTime() const { return lastAnimTime; }

	void SetCurrentFrame(int cFrame) { currentFrame = cFrame; }
	void SetLastAnimTime(unsigned long t) { lastAnimTime = t; }

	bool UpdateRhythm(); 
	bool IsBeatWindow(); 
	void FinishRhythm();

	R_Note* GetCurrentNote() { return Note == nullptr ? nullptr : Note; }
	float GetRatio() const;
};

