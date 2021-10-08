/*
    Author: baihan 
    class purpose: hand hit event once every animation 
*/
#pragma once
#include "CoreMinimal.h"
#include "CombatSolution.h"
#include "CombatSolution_Once.generated.h"

UCLASS(Blueprintable)
class MELEECORE_API UCombatSolution_Once : public UObject, public ICombatSolution
{
    GENERATED_BODY()

private:
    TArray<AActor *> m_Actors;

public:
    ~UCombatSolution_Once();
    virtual void OnHit_Implementation(AActor* hitter, AActor *attackedActor, ECombatHitResult &outResult) override;

    virtual void InitData_Implementation() override;

    virtual void OnStartDetection_Implementation() override;

    virtual void OnEndDetection_Implementation() override;
};