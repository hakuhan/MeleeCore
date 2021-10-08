#pragma once

#include "CoreMinimal.h"
#include "Tickable.h"
#include "SkillComponent/SkillInfo.h"
#include "SkillComponent/Skill.h"
#include "SkillComponent/SkillLineInfo.h"
#include "SkillLine.generated.h"


UENUM(BlueprintType)
enum class ESkillLineState : uint8
{
    SKILL_LINE_UNSTART = 0 UMETA(DisplayName = "未开始"),
    SKILL_LINE_PLAYING UMETA(DisplayName = "正在使用"),
    SKILL_LINE_JUMPED UMETA(DisplayName = "跳转"),
    SKILL_LINE_FINISHED UMETA(DisplayName = "结束"),
};

DECLARE_DELEGATE_OneParam(FSkillLineStateChangeEvent, ESkillLineState);

USTRUCT(BlueprintType)
struct MELEECORE_API FSkillLineData
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    FSkillLineInfo Record;

    UPROPERTY(BlueprintReadOnly)
    int SkillOffset = 0;

    bool IsEnable = false;

    UPROPERTY(BlueprintReadOnly)
    ESkillLineState State;
};

UCLASS()
class MELEECORE_API USkillLine : public UObject, public FTickableGameObject
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    FSkillLineInfo m_Info;
    UPROPERTY(BlueprintReadWrite)
    FSkillLineData m_Data;

    UPROPERTY(BlueprintReadOnly)
    AActor* m_Target;

    UPROPERTY(BlueprintReadOnly)
    USkill* m_Skill;

    UPROPERTY(BlueprintReadOnly)
    ASkillDynamicData* m_DynamicData;

    FSkillLineStateChangeEvent OnChangeState;
    OnSkillEndDelegate OnSkillLineEnd;
public:
    // Begin FTickableGameObject Interface.
    virtual void Tick(float DeltaTime) override;
    virtual bool IsTickable() const override { return m_Data.IsEnable; }
    virtual TStatId GetStatId() const override { return TStatId(); }

public:
    UFUNCTION(BlueprintCallable)
    void UpdateInfo(const FSkillLineInfo &info, AActor* target, ASkillDynamicData* dynamicData);

    // Empty string means from begin
    UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "skillName"))
    bool StartLine(const FString& skillName);

    UFUNCTION(BlueprintCallable)
    bool GetCurrentSkillName(FString& outName);

    UFUNCTION(BlueprintCallable)
    bool GetNextSkillName(FString& outName);

    UFUNCTION(BlueprintCallable)
    bool SwitchSkill(const FString &skillName);

    UFUNCTION(BlueprintCallable)
    bool IsExecuting();

    UFUNCTION(BlueprintCallable)
    bool IsEndSkillLine();

    UFUNCTION(BlueprintCallable)
    void FinishSkill(bool terminate, const FAlphaBlend& InBlendOut);

    UFUNCTION(BlueprintCallable)
    void OnSwitchOut();

    UFUNCTION(BlueprintCallable)
    void GetSkillRecord(FSkillLineInfo& records);

    UFUNCTION(BlueprintCallable)
    bool CanSwitch();

    UFUNCTION(BlueprintCallable)
    bool EndLooping();

protected:
    bool GetCurrentSkillInfo(FSkillInfo& outInfo);
    bool SwitchSkillByIndex(int index);
    bool IsCurrentSkillOver();
    void UpdateState(ESkillLineState state);

    // return true if create a new skill
    USkill* UpdateSkill(const FSkillInfo& skillInfo);
};