#pragma once

#include "CoreMinimal.h"
#include "MeleeConditionalData.h"

#include "Life.generated.h"

UCLASS(Blueprintable, classGroup = (MeleeCore), meta = (BlueprintSpawnableComponent))
class MELEECORE_API ULife : public USceneComponent
{
    GENERATED_BODY()
public:
    ULife();

    UPROPERTY(EditAnywhere, Instanced, BlueprintReadWrite)
    UMeleeConditionalData* m_LifeHandler;
};