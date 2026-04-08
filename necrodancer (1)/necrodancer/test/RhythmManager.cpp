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
		if (ratio < RHYTHM_MARGIN || ratio > (1.0f - RHYTHM_MARGIN))
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

float RhythmManager::GetRatio() const
{
	if (Note == nullptr) return 0.0f;
	return Note->Getcurrenttime() / Note->Getmaxtime();
}
