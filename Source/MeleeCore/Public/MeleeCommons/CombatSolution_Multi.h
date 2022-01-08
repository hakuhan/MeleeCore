/*
    Author: baihan 
    class purpose: Handle multi attack per animation 
*/

#pragma once

#include "CoreMinimal.h"
#include "CombatSolution.h"
#include "TimerManager.h"
#include "Actor_Countdown.h"
#include "CombatSolution_Multi.generated.h"

USTRUCT(BlueprintType)
struct MELEECORE_API FHurtMulti : public FHurt
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float interval;

public:
	~FHurtMulti()
	{
	}
};

UCLASS()
class UCombatSolution_Multi : public UObject, public ICombatSolution
{
    GENERATED_BODY()

private:
    TArray<AActor *> m_Actors;

public:
    ~UCombatSolution_Multi();
    /*hit interval of multi attack*/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Solution")
    float m_MeleeInterval = 0.1f;

private:
    UPROPERTY()
    TMap<AActor*, FTimerHandle> m_IntervalTimers;

public:
    virtual void OnHit_Implementation(AActor* hitter, AActor *attackedActor, ECombatHitResult &outResult) override;

    virtual void InitData_Implementation() override;

    virtual void OnStartDetection_Implementation() override;

    virtual void OnEndDetection_Implementation() override;

    virtual void UpdateHurts(FHurt data) override;

    UFUNCTION(BlueprintCallable)
    bool IsCooling(AActor* hittedActor);

protected:
    void CoolOver();
};