#pragma once

#include "CoreMinimal.h"
#include "Tickable.h"
#include "Animation/AnimMontage.h"
#include "SkillComponent/SkillInfo.h"
#include "SkillDynamicData.h"
#include "Skill.generated.h"

UENUM(BlueprintType)
enum class ESkillExecuteState : uint8
{
    SKILL_EXECUTE_NONE UMETA(DisplayName="未执行"),
    SKILL_EXECUTE_WORK UMETA(DisplayName="执行中"),
    SKILL_EXECUTE_SWITCHABLE UMETA(DisplayName="可切换"),
    SKILL_EXECUTE_END UMETA(DisplayName="结束"),
};

USTRUCT(BlueprintType)
struct FSkillData
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    ESkillExecuteState ExecuteState;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    ESkillBreakLevel BreakState;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    bool IsEnable = false;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    float TimeLeft;

    FSkillData() 
    {
        ExecuteState = ESkillExecuteState::SKILL_EXECUTE_NONE;
        BreakState = ESkillBreakLevel::SKILL_BREAK_EASY;
        IsEnable = false;
        TimeLeft = 0;
    }
};

UCLASS()
class MELEECORE_API USkill : public UObject, public FTickableGameObject
{
    GENERATED_BODY()

public:
    // Begin FTickableGameObject Interface.
    virtual void Tick(float DeltaTime) override;
    virtual bool IsTickable() const override { return m_Data.IsEnable; }
    virtual TStatId GetStatId() const override { return TStatId(); }

public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    FSkillInfo m_Info;
    UPROPERTY(BlueprintReadWrite)
    FSkillData m_Data;
    UPROPERTY()
    UAnimInstance* m_TargetAnim;
    UPROPERTY(BlueprintReadOnly)
    ASkillDynamicData* m_DynamicData;

    void UpdateData(const FSkillInfo& info, ASkillDynamicData* dynamicData);

    ESkillExecuteState GetState()
    {
        return m_Data.ExecuteState;
    }

    UFUNCTION(BlueprintCallable)
    void ExecuteSkill(AActor* target);

    bool IsMatch(const FSkillInfo& info);

    bool CanSwitch();

    void Terminate();

    void Stop(const FAlphaBlend& InBlendOut = FAlphaBlend(0.2f));

    bool EndLooping();

};