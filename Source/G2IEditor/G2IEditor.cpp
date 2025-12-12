#include "G2IEditor.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_MODULE(FG2IEditorModule, G2IEditor);

DEFINE_LOG_CATEGORY(LogG2I)

#define LOCTEXT_NAMESPACE "G2IEditor"

void FG2IEditorModule::StartupModule()
{
    UE_LOG(LogG2I, Warning, TEXT("G2IEditor: Log Started"));
}

void FG2IEditorModule::ShutdownModule()
{
    UE_LOG(LogG2I, Warning, TEXT("G2IEditor: Log Ended"));
}

#undef LOCTEXT_NAMESPACE