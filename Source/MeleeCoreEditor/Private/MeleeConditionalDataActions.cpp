#include "MeleeConditionalDataActions.h"
#include "MeleeCommons/MeleeConditionalData.h"

#define LOCTEXT_NAMESPACE "MeleeConditionalData"

FColor FMeleeConditionalDataActions::GetTypeColor() const
{
    return FColor(255, 123, 123);
}

FText FMeleeConditionalDataActions::GetName() const
{
    return LOCTEXT("MeleeConditionalAssetTypeActions", "MeleeConditionalData");
}

void FMeleeConditionalDataActions::OpenAssetEditor(const TArray<UObject *> &InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor)
{
    FSimpleAssetEditor::CreateEditor(EToolkitMode::Standalone, EditWithinLevelEditor, InObjects);
}

UClass *FMeleeConditionalDataActions::GetSupportedClass() const
{
    return UMeleeConditionalData::StaticClass();
}

uint32 FMeleeConditionalDataActions::GetCategories()
{
    return EAssetTypeCategories::Type::Gameplay;
}

bool FMeleeConditionalDataActions::HasActions(const TArray<UObject*>& InObjects) const
{
    return true;
}

// void FMeleeConditionalDataActions::GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder)
// {
// 	FAssetTypeActions_Base::GetActions(InObjects, MenuBuilder);

// 	auto assets = GetTypedObjectPtrs<UMeleeConditionalData>(InObjects);

//     MenuBuilder.AddMenuEntry(LOCTEXT("StoryEditor", "Open Story Editor"), 
//         LOCTEXT("StorySystemToolTip", "Open Story Editor To Edit Story"),
//         FSlateIcon(FSlateIcon()),
// 		FUIAction(
// 			FExecuteAction::CreateLambda([=] {
// 				// Open story editor
// 				if (InObjects.Num() > 0 && InObjects[0] != nullptr)
// 				{
// 					UMeleeConditionalData* asset = dynamic_cast<UMeleeConditionalData*>(InObjects[0]);
// 					if (asset)
// 					{
// 						UStoryEditorFunctionLibrary::OnOpenEditor(asset);
// 					}
// 				}
// 				}),
// 			FCanExecuteAction::CreateLambda([=] { // ѡ�е�Object������һ������nullptr����Ҫִ�д�������.
// 				for (auto asset : assets)
// 				{
// 					if (asset == nullptr)
// 					{
// 						return false;
// 					}
// 				}
// 				return true;
// 				}))
// 		);
// }

#undef LOCTEXT_NAMESPACE
