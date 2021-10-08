/*
    Author: baihan 
    Date: "2021-02-18" 
    Class discription: Provide basic motion rules 
*/

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MotionSwitchRule.h"
#include "MotionSwitchRuleBase.generated.h"

DECLARE_DYNAMIC_DELEGATE_RetVal(FMotionCheckResult, FMotionRuleDelegate);

USTRUCT(BlueprintType)
struct FMotionRuleEvent
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=MotionRuleEvent)
    FMotionRuleDelegate Callback;
};

USTRUCT(BlueprintType)
struct FMotionInfo
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    int Id;
    UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    TArray<int> SwitchableMotionIds;
    UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    TArray<int> ForbiddenMotionIds;
    UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
    bool SwitchSelf;
};

UCLASS(Blueprintable, ClassGroup = (MeleeCore))
class MELEECORE_API UMotionSwitchRuleBase : public UActorComponent, public IMotionSwitchRule
{
    GENERATED_BODY()

public:
#pragma region manage motion and rule data
    UFUNCTION(BlueprintCallable)
    void InitMotionInfo(const TMap<int, FMotionInfo>& info, int allId, int noneId)
    {
        m_Info = info;
        m_All = allId;
        m_None = noneId;
        Execute_SwitchMotion(this, m_All);
    }

    UFUNCTION(BlueprintCallable)
    void UpdateAllMotions(const TArray<int>& switchMotions, const TArray<int>& forbiddenMotions);

    UFUNCTION(BlueprintCallable)
    bool AddMotion(int Id);

    UFUNCTION(BlueprintCallable)
    void RemoveMotion(int Id);

    UFUNCTION(BlueprintCallable)
    void UpdateAlternativeRules(const TMap<int, FMotionRuleEvent>& rules);

    UFUNCTION(BlueprintCallable)
    void UpdateBasicRule(int id, const FMotionRuleDelegate& event);
    
    UFUNCTION(BlueprintCallable)
    void RemoveBasicRule(int id);
#pragma endregion


#pragma region logic
    virtual FMotionCheckResult IsSwitchable_Implementation(int id) override;

    virtual void SwitchMotion_Implementation(int id) override;

    virtual void EndMotion_Implementation(int id) override;

    UFUNCTION(BlueprintCallable)
    bool CheckMotion(int id);

    UFUNCTION(BlueprintCallable)
    void CheckRule(int id, FMotionCheckResult& outResult);
#pragma endregion


public:
    UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Rule info")
    TMap<int, FMotionInfo> m_Info;
    UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Rule info")
    TArray<int> m_SwitchList;
    UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Rule info")
    TArray<int> m_ForbiddenList;

    UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Rule info")
    TMap<int, FMotionRuleDelegate> m_BasicRules;
    UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Rule info")
    TMap<int, FMotionRuleDelegate> m_AlternativeRules;

private:
    int m_All = -1;
    int m_None = -1;
    int m_CurrentMotion = -1;
    bool m_SwitchSelf = false;

};