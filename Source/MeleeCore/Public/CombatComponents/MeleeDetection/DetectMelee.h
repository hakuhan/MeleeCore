// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/ActorComponent.h"
#include "MeleeDetection/DetectSolution.h"
#include "Engine/DataTable.h"
#include "MeleeEffect.h"
#include "DetectTypes.h"
#include "CombatSolution.h"
#include "MeleeWeapon.h"
#include "DetectMelee.generated.h"


UENUM(BlueprintType, Meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EDetectType : uint8
{
	NONE = 0 UMETA(Hidden),
	RAY = 1 << 0 UMETA(DisplayName="Ray Check"),
	COLLISION = 1 << 1 UMETA(DisplayName="Collision Check"),
};

/*
* weapon hurt type
*/
UENUM(BlueprintType)
enum class EHurtType : uint8
{
	ONCE_SOLUTION,
	MULTI_SOLUTION
};

USTRUCT(BlueprintType)
struct MELEECORE_API FDetectSolutionTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EDetectType DetectType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MustImplement = "DetectSolution"))
	TSubclassOf<UObject> Class;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDataTableRowHandle DetectInfo;
};

// Melee structure
USTRUCT(BlueprintType)
struct MELEECORE_API FHurtSolutionTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EHurtType solutionType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MustImplement = "CombatSolution"))
	TSubclassOf<UObject> solutionClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDataTableRowHandle hurtTable;
};

UCLASS(Blueprintable, ClassGroup = (MeleeCore), meta = (BlueprintSpawnableComponent))
class MELEECORE_API UDetectMelee : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY()
	TScriptInterface<ICombatSolution> m_HurtSolution;
	UPROPERTY()
	TArray<AActor *> m_HitActorTemps;
	UPROPERTY()
	uint8 m_WeaponMask;

#pragma region Tick caches
	UPROPERTY()
	TArray<FDetectInfo> m_DetectTemp;

	UPROPERTY()
	FMeleeWeaponInfo m_WeaponInfoTemp;
	UPROPERTY()
	FWeaponData m_WeaponDataTemp;
#pragma endregion

public:
#pragma region Solutions
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Melee")
	EDetectType m_DetectType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Melee")
	TScriptInterface<IDetectSolution> m_DetectSolution;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Melee")
	EMeleeHurt m_DefaultHurt;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Melee")
	EMeleeHurt m_Hurt;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Melee")
	EHurtType m_DefaultHurtSolutionType;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Melee")
	EHurtType m_HurtSolutionType;
#pragma endregion

	// Detect solution
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee")
	FDataTableRowHandle m_DetectSolutionTable;

	// Hurt solution
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee")
	FDataTableRowHandle m_HurtSolutionTable;

	// Effects
	UPROPERTY(VisibleAnywhere, Category = "Melee")
	UMeleeEffect *m_EffectComponent;

	// Weapon data
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Melee")
	TArray<TScriptInterface<IMeleeWeapon>> m_MeleeWeapons;

	UPROPERTY(VisibleAnywhere, Category = "Melee")
	bool m_IsDetecting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Melee")
	bool m_IsDebug = false;

	UPROPERTY()
	float m_DetectTiming = 0;

protected:
	void BeginPlay() override;
	void BeginDestroy() override;

public:
	// Sets default values for this component's properties
	UDetectMelee();

#pragma region HurtControl
	/*
	Change hurt
	*/
	UFUNCTION(BlueprintCallable)
	void UpdateHurts(EMeleeHurt newHurt, EHurtType newSolution);

	/* Change hurt rate */
	UFUNCTION(BlueprintCallable)
	void UpdateHurtRate(float rate);

	UFUNCTION(BlueprintCallable)
	void UpdateWeaponMask(uint8 weaponMask);

	/* Reset hurt to default */
	UFUNCTION(BlueprintCallable)
	void ResetHurts();
#pragma endregion

#pragma region DetectControl
	UFUNCTION(BlueprintCallable)
	void UpdateDetect(EDetectType type);

	UFUNCTION(BlueprintCallable)
	void UpdateCollisionData(const FCollisionDetectData& data);

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
	// Detection
	UFUNCTION(BlueprintCallable)
	void StartDetection(float duration);

	UFUNCTION(BlueprintCallable)
	void EndDetection();

	UFUNCTION(BlueprintCallable)
	void ResetData();

	UFUNCTION(BlueprintCallable)
	void GetWeapons(TArray<TScriptInterface<IMeleeWeapon>>& outWeapons)
	{
		outWeapons = m_MeleeWeapons;
	}

	/// <summary>
	/// Update weapon and slot info
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void UpdateWeapon();
#pragma endregion

private:
	void ExecuteHit(FDetectInfo& hit);
};
