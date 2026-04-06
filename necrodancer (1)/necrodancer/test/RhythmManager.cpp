#include "RhythmManager.h"
#include "R_Note.h"

void RhythmManager::SetNoteList(float bpm)
{
	Note = new R_Note(bpm);
	Note->StartNote();
}

bool RhythmManager::UpdateRhythm()
{
	if (Note==nullptr) return false;
		
	bool isBeat = false;

		bool temp = false;
		Note->UpdateNoteTime(temp);
		if (temp) isBeat = true;


	if (Note!= nullptr) {
		float ratio = Note->Getcurrenttime() / Note->Getmaxtime();
		if (ratio < 0.15f || ratio > 0.85f)
			SetCurrentFrame(1);
		else
			SetCurrentFrame(0);
	}

	return isBeat;
}

bool RhythmManager::IsBeatWindow()
{
	if (Note == nullptr) return false;
	return Note->OnNoteInputCompleted();
}

void RhythmManager::FinishRhythm()
{
	if (Note == nullptr) return;
	
	delete Note;
}
