// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatHitEffect.h"
#include "MeleeUtils.h"
#include "MeleeEffect.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MELEECORE_API UMeleeEffect : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UMeleeEffect();

protected:
	UPROPERTY()
	TArray<AActor*> m_HittedActors;
	UPROPERTY(BlueprintReadWrite, Category = "Effect")
	TArray<UObject *> m_HitEffects;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MustImplement = "CombatHitEffect"), Category = "Effect")
	TArray<TSubclassOf<UObject>> m_HitEffectClasses;

public:
	void BeginPlay() override;
	void BeginDestroy() override;

	UFUNCTION(BlueprintCallable, Category = "Effect")
	void OnCombatHitEffect(FDetectInfo hitInfo, ECombatHitResult hitType);

#pragma region ManageEffect
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Effect")
	void InitEffects();

	UFUNCTION(BlueprintCallable, Category = "Effect")
	void InitFromOwner();

	UFUNCTION(BlueprintCallable, Category = "Effect")
	void InitFromComponents();

	// Init by classes link
	UFUNCTION(BlueprintCallable, Category = "Effect")
	void InitEffectClasses();

	UFUNCTION(BlueprintCallable, Category = "Effect")
	void AddHitEffect(UObject *effect);

	UFUNCTION(BlueprintCallable, Category = "Effect")
	void RemoveHitEffect(UObject *effect);

	UFUNCTION(BlueprintCallable, Category = "Effect")
	void ResetData();

#pragma endregion ManageEffect

protected:
	// check array's element is valid
	UFUNCTION(BlueprintCallable)
	void CheckEffectClass(TArray<UObject *> &arrays);
};
