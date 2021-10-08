#pragma once

#include "CoreMinimal.h"

#include "CollisionDetectInfo.generated.h"

UENUM(BlueprintType, Meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class ECollisionDetectType : uint8
{
    NONE = 0 UMETA(Hidden),
    SPHERE_DETECT = 1 << 0 UMETA(DisplayName="Sphere"),
    CAPSULE_DETECT = 1 << 1 UMETA(DisplayName="Capsule"),
    BOX_DETECT = 1 << 2 UMETA(DisplayName="Box"),
};

USTRUCT(BlueprintType)
struct FCollisionDetectInfo : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=CollisionDetect)
    TMap<ECollisionDetectType, TSubclassOf<UShapeComponent>> CollisionClassMaps;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=CollisionDetect)
    FName CollisionPresetName = TEXT("PlayerCombat");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CollisionDetect, meta = (Bitmask, BitmaskEnum = "ECollisionChannel"))
	TEnumAsByte<ECollisionChannel> Channel;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=CollisionDetect)
    bool IsShowCollision;

};
