#include "MeleeCoreEditor.h"
#include "MeleeConditionalDataActions.h"

#define LOCTEXT_NAMESPACE "FMeleeCoreEditorModule"

void FMeleeCoreEditorModule::StartupModule()
{
    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	AssetTools.RegisterAssetTypeActions(MakeShareable(new FMeleeConditionalDataActions()));
}

void FMeleeCoreEditorModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMeleeCoreEditorModule, MeleeCoreEditor)