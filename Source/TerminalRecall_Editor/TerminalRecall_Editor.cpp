#include "TerminalRecall_Editor.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_MODULE(FTerminalRecall_EditorModule, TerminalRecall_Editor)

void FTerminalRecall_EditorModule::StartupModule() 
{
	FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");

	TSharedRef<FPropertySection> mySection = PropertyModule.FindOrCreateSection("BaseEntity", "StuffForDesigners", INVTEXT("Stuff for Designers"));
	mySection->AddCategory("Stuff for Designers");
	
	mySection = PropertyModule.FindOrCreateSection("BaseGridCell", "StuffForDesigners", INVTEXT("Stuff for Designers"));
	mySection->AddCategory("Stuff for Designers");
}

void FTerminalRecall_EditorModule::ShutdownModule() 
{
	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.RemoveSection("ABaseEntity", "StuffForDesigners");
	}
}
