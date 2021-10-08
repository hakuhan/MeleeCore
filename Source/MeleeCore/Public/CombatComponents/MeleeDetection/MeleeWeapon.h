/*
	Author: baihan 
	class purpose: Mark Melee weapon 
*/

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "MeleeDetection/MeleeWeaponInfo.h"
#include "MeleeWeapon.generated.h"

USTRUCT(BlueprintType)
struct FWeaponData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=MeleeWeapon)
	TArray<FVector> TempSocketLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MeleeWeapon)
	bool IsEnabled = true;
public:
	FWeaponData() 
	{
		IsEnabled = true;
	}
	FWeaponData(bool isEnabled) 
	{
		IsEnabled = isEnabled;
	}
	~FWeaponData()
	{
		TempSocketLocation.Empty();
	}
};

UINTERFACE()
class MELEECORE_API UMeleeWeapon : public UInterface
{
	GENERATED_BODY()
};

class MELEECORE_API IMeleeWeapon
{
	GENERATED_BODY()

public:
	virtual bool GetInfo(FMeleeWeaponInfo& outInfo) = 0;
	virtual bool GetData(FWeaponData& outData) = 0;
	virtual void SetData(const FWeaponData& inData) = 0;
	virtual bool IsWeaponEnabled()
	{
		return true;
	}
	virtual void SetWeaponEnabled(bool enable) = 0;

	virtual FVector GetDetectLocation(FName SocketName) = 0;
	bool IsTargetWeapon(uint8 WeaponMask)
	{
		bool result = false;

		FMeleeWeaponInfo _info; 
		result = GetInfo(_info);
		if (result)
		{
			result = _info.WeaponType & WeaponMask;
		}
		
		return result;
	}
};
