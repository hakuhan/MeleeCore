#pragma once

#include "CoreMinimal.h"

#include "MeleeConditionalData.generated.h"

DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(bool, FCheckingDelegate, float, conditionValue);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnConditionalDataUpdated, float, newValue);

UCLASS(BlueprintType)
class MELEECORE_API UMeleeConditionalData : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Data;

    UPROPERTY()
    TArray<FCheckingDelegate> IncreaseConditions;
    
    UPROPERTY()
    TArray<FCheckingDelegate> DecreaseConditions;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FOnConditionalDataUpdated OnIncreased;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FOnConditionalDataUpdated OnDecreased;

public:

    UFUNCTION(BlueprintCallable)
    void InitData(float newData)
    {
        Data = newData;
    }

    UFUNCTION(BlueprintCallable)
    float GetData()
    {
        return Data;
    }

    UFUNCTION(BlueprintCallable)
    bool Increase(float deltaValue)
    {
        for (int i = 0; i < IncreaseConditions.Num(); ++i)
        {
            if (!IncreaseConditions[i].Execute(deltaValue))
            {
                return false;
            }
            
        }
        
        Data += deltaValue;
        OnIncreased.Broadcast(Data);
        return true;
    }

    UFUNCTION(BlueprintCallable)
    bool Decrease(float deltaValue)
    {
        for (int i = 0; i < DecreaseConditions.Num(); ++i)
        {
            if (!DecreaseConditions[i].Execute(deltaValue))
            {
                return false;
            }
            
        }
        
        Data -= deltaValue;
        OnDecreased.Broadcast(Data);
        return true;
    }

    UFUNCTION(BlueprintCallable)
    int RegistIncreaseCheckingEvent(const FCheckingDelegate& increaseChecking)
    {
        IncreaseConditions.Add(increaseChecking);

        return IncreaseConditions.Num() - 1;
    }

    UFUNCTION(BlueprintCallable)
    bool UnRegistIncreaseCheckingEvent(int eventIndex)
    {
        if (IncreaseConditions.IsValidIndex(eventIndex))
        {
            IncreaseConditions.RemoveAt(eventIndex);
            return true;
        }
        
        return false;
    }

    UFUNCTION(BlueprintCallable)
    int RegistDecreaseCheckingEvent(const FCheckingDelegate& decreaseChecking)
    {
        DecreaseConditions.Add(decreaseChecking);

        return DecreaseConditions.Num() - 1;
    }

    UFUNCTION(BlueprintCallable)
    bool UnRegistDecreaseCheckingEvent(int eventIndex)
    {
        if (DecreaseConditions.IsValidIndex(eventIndex))
        {
            DecreaseConditions.RemoveAt(eventIndex);
            return true;
        }
        
        return false;
    }
};
