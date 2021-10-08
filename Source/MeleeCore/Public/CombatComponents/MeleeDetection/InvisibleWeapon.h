#pragma once

#include "CoreMinimal.h"
#include "MeleeDetection/MeleeWeapon.h"
#include "InvisibleWeapon.generated.h"

UCLASS(ClassGroup = (MeleeCore), meta = (BlueprintSpawnableComponent))
class MELEECORE_API UInvisibleWeapon : public USceneComponent, public IMeleeWeapon
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName m_SocketName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMeleeWeaponInfo m_Info;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FWeaponData m_Data;

protected:
	UInvisibleWeapon()
	{
		PrimaryComponentTick.bCanEverTick = false;
		m_SocketName = TEXT("InvisibleWeapon");
	}
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual FVector GetDetectLocation(FName SocketName) override
	{
		return GetComponentLocation();
	}
	virtual bool GetInfo(FMeleeWeaponInfo& outInfo) override
	{
		outInfo = m_Info;
		return true; 
	}

	virtual bool GetData(FWeaponData& outData) override
	{
		outData = m_Data;
		return true;
	}
	virtual void SetData(const FWeaponData& inData) override
	{
		m_Data = inData;
	}
	virtual bool IsWeaponEnabled() override
	{
		return m_Data.IsEnabled;
	}
	virtual void SetWeaponEnabled(bool enable) override
	{
		m_Data.IsEnabled = enable;
	}
};