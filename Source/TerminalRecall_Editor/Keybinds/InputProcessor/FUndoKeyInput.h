#pragma once
#include "Framework/Application/IInputProcessor.h"

class UEncounterLevelMakerWindow;

class FUndoKeyInput : public IInputProcessor
{
public:
	FUndoKeyInput(UEncounterLevelMakerWindow* InWidget) 
		: TargetWidget(InWidget) {}
	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override {};
	virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override;
	virtual const TCHAR* GetDebugName() const override { return TEXT("FGlobalEditorInputProcessor"); }
private:
	TWeakObjectPtr<UEncounterLevelMakerWindow> TargetWidget;
};
