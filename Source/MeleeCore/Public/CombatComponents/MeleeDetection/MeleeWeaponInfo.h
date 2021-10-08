#pragma once

#include "CoreMinimal.h"
#include "MeleeWeaponInfo.generated.h"

UENUM(BlueprintType, Meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EAttackWeapon : uint8
{
	NONE = 0 UMETA(Hidden),
	Melee_LeftHand = 1 << 0 UMETA(DisplayName="Left Hand"),
	Melee_RightHand = 1 << 1 UMETA(DisplayName="Right Hand"),
	Melee_LeftFoot = 1 << 2 UMETA(DisplayName="Left Foot"),
	Melee_RightFoot = 1 << 3 UMETA(DisplayName="Righ Foot"),
};

USTRUCT(BlueprintType)
struct MELEECORE_API FMeleeWeaponInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Bitmask, BitmaskEnum="EAttackWeapon"))
	uint8 WeaponType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<FName> SocketNames;

public:
	FMeleeWeaponInfo() {}
	~FMeleeWeaponInfo()
	{
		SocketNames.Empty();
	}
};