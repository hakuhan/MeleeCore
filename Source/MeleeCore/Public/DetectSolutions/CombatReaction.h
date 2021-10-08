/*
    Author: baihan 
    class purpose: Melee Reaction API 
*/
#pragma once

#include "CoreMinimal.h"
#include "DetectTypes.h"
#include "CombatReaction.generated.h"

UINTERFACE(Blueprintable)
class MELEECORE_API UCombatReaction : public UInterface
{
    GENERATED_BODY()
};

class MELEECORE_API ICombatReaction
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent)
    void OnMeleeHitted(float hurts);

    UFUNCTION(BlueprintNativeEvent)
    ECombatHitResult GetHitResult();

};