/*
    Author: baihan 
    class purpose: Interface for melee solutoin 
*/
#pragma once
#include "CombatReaction.h"
#include "Engine/DataTable.h"
#include "MeleeUtils.h"
#include "DetectTypes.h"
#include "CombatSolution.generated.h"

USTRUCT(BlueprintType)
struct MELEECORE_API FHurt : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMeleeHurt hurtType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float hurts;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool briefHitter = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float briefDuration = 0.038f;

public:
	~FHurt()
	{
	}
};

UINTERFACE(Blueprintable)
class MELEECORE_API UCombatSolution : public UInterface
{
    GENERATED_BODY()
};

class MELEECORE_API ICombatSolution
{
    GENERATED_BODY()

public:
    FHurt m_HurtInfo;
    float m_HurtRate = 1;

public:
    // Attacking
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void OnHit(AActor* hitter, AActor *attackedActor, ECombatHitResult &outResult);

    // init
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void InitData();

    void Init(FHurt data)
    {
        UpdateHurts(data);
        this->Execute_InitData(dynamic_cast<UObject*>(this));
    }

    virtual void UpdateHurts(FHurt data)
    {
        m_HurtInfo = data;
    }

    void UpdateHurtRate(float rate)
    {
        m_HurtRate = rate;
    }

    // start event
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void OnStartDetection();

    // end event
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void OnEndDetection();

protected:
    void NoticeHit(AActor* hitter, AActor *hittedActor, ECombatHitResult &outResult)
    {
        if (m_HurtInfo.briefHitter)
        {
            UMeleeUtils::BriefPauseMontage(hitter, m_HurtInfo.briefDuration);
        }

        TArray<UObject *> reacters;
        UMeleeUtils::GetImplementFromActor(hittedActor, UCombatReaction::StaticClass(), reacters);
        if (reacters.Num() > 0)
        {
            for (auto reacter : reacters)
            {
                ICombatReaction::Execute_OnMeleeHitted(reacter, m_HurtInfo.hurts * m_HurtRate);
                outResult = ICombatReaction::Execute_GetHitResult(reacter);
            }
        }
    }
};