/*
    Author: baihan 
    Date: "2021-02-01" 
    Class discription: Ruler of motion
*/

#pragma once

#include "CoreMinimal.h"
#include "MotionSwitchRule.generated.h"

USTRUCT(BlueprintType)
struct FMotionCheckResult
{
    GENERATED_USTRUCT_BODY()

        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MotionRule)
        bool bSwitchable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MotionRule)
        FString CurrentActionName;

    FMotionCheckResult()
        : bSwitchable(false)
        , CurrentActionName("")
    {}
    FMotionCheckResult(bool switchable, const FString& currentActionName)
        : bSwitchable(switchable)
        , CurrentActionName(currentActionName)
    {}
};

UINTERFACE()
class MELEECORE_API UMotionSwitchRule : public UInterface
{
    GENERATED_BODY()
};

class MELEECORE_API IMotionSwitchRule
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    FMotionCheckResult IsSwitchable(int id);

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void SwitchMotion(int id);

    UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    void EndMotion(int id);
};
