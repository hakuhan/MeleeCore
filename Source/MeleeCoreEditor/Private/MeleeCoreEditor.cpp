#include "MeleeCoreEditor.h"
#include "MeleeConditionalDataActions.h"

#define LOCTEXT_NAMESPACE "FMeleeCoreEditorModule"

void FMeleeCoreEditorModule::StartupModule()
{
    // Conditoinal data editor
    IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	AssetTools.RegisterAssetTypeActions(MakeShareable(new FMeleeConditionalDataActions()));

    // Custom panel
    // FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    // PropertyModule.RegisterCustomClassLayout(UMeleeConditionalData::StaticClass()->GetFName(), FOnGetDetailCustomizationInstance::CreateStatic(&FMeleeConditionalDataCustomization::MakeInstance));
}

void FMeleeCoreEditorModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FMeleeCoreEditorModule, MeleeCoreEditor)