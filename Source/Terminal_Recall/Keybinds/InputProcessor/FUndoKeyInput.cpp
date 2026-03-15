#include "FUndoKeyInput.h"

#include "Editor/Transactor.h"
#include "Terminal_Recall/EncounterRelatedCode/EncounterLevelMakerWindow.h"

bool FUndoKeyInput::HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent)
{
	if (TargetWidget.IsValid())
	{
		// Detect "Ctrl + Z"
		if (InKeyEvent.GetKey() == EKeys::Z && InKeyEvent.IsControlDown())
		{
			return TargetWidget->ActualUndo();
		}
		// Detect "Ctrl + Y"
		if (InKeyEvent.GetKey() == EKeys::Y && InKeyEvent.IsControlDown())
		{
			if (GEditor->Trans->CanRedo())
				return false;
			return TargetWidget->ActualRedo();
		}
	}
	return false;
}
