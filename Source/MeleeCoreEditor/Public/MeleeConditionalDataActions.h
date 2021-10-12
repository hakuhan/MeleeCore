#pragma once

#include "CoreMinimal.h"
#include "AssetTypeActions_Base.h"

class FMeleeConditionalDataActions : public FAssetTypeActions_Base
{
public:
	virtual FColor GetTypeColor() const override;
    
    virtual void OpenAssetEditor(const TArray<UObject *> &InObjects, TSharedPtr<class IToolkitHost> EditWithinLevelEditor = TSharedPtr<IToolkitHost>()) override;

	virtual FText GetName() const override;

	virtual UClass* GetSupportedClass() const;

	virtual uint32 GetCategories();

	bool HasActions(const TArray<UObject*>& InObjects) const override;

	// void GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder) override;

};
