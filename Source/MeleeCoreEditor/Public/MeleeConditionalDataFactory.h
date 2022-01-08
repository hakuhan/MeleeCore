#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "AssetTypeCategories.h"
#include "MeleeCommons/MeleeConditionalData.h"

#include "MeleeConditionalDataFactory.generated.h"

UCLASS()
class MELEECOREEDITOR_API UMeleeConditionalDataFactory : public UFactory
{
    GENERATED_BODY()
public:
    UMeleeConditionalDataFactory()
    {
        bCreateNew = true;
        bEditAfterNew = true;
        SupportedClass = UMeleeConditionalData::StaticClass();
    }

    virtual UObject *FactoryCreateNew(UClass *InClass, UObject *InParent, FName InName, EObjectFlags Flags, UObject *Context, FFeedbackContext *Warn, FName CallingContext)
    {
        return NewObject<UMeleeConditionalData>(InParent, InName, Flags | RF_Transactional);
    }

    virtual uint32 GetMenuCategories() const override
    {
        return EAssetTypeCategories::Type::Gameplay;
    }
};