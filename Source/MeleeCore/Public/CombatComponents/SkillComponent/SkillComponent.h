#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SkillComponent/SkillLineInfo.h"
#include "SkillComponent/Skill.h"
#include "SkillComponent/SkillLine.h"
#include "SkillComponent/SkillDynamicData.h"
#include "SkillComponent.generated.h"

UENUM(BlueprintType)
enum class ESkillState : uint8
{
	SKILL_UNSTART = 0 UMETA(DisplayName = "未开始"),
	SKILL_EXECUTING UMETA(DisplayName = "执行中"),
	SKILL_TERMINAl UMETA(DisplayName = "终止"),
};

USTRUCT(BlueprintType)
struct FSkillTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Skill)
		TArray<FSkillLineInfo> SkillLines;
};

USTRUCT(BlueprintType)
struct FSkillComponentData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere)
		int LineIndex = -1;
	UPROPERTY(BlueprintReadOnly)
		ESkillState State = ESkillState::SKILL_UNSTART;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		ASkillDynamicData* DynamicData;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		bool bAutoPlay;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		bool bSwitch;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		int SwitchLineIndex;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		FString SwitchSkillName;

	FSkillComponentData()
	{ }

	FSkillComponentData(bool bDebug)
		: DynamicData(NewObject<ASkillDynamicData>())
	{
		DynamicData->bDebug = bDebug;
	}
};

UCLASS(Blueprintable, ClassGroup = (MeleeCore))
class MELEECORE_API USkillComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
		bool m_debug = false;
	UPROPERTY(BlueprintReadWrite)
		FSkillTable m_Info;
protected:
	UPROPERTY()
		USkillLine* m_LineControl;
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
		FSkillComponentData m_Data;

public:
	void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
		void InitInfo();

#pragma region Execute
	// Execute current line skill
	UFUNCTION(BlueprintCallable)
		bool ExecuteSkill();

	UFUNCTION(BlueprintCallable)
		bool GetCurrentSkillName(FString& outName);

#pragma endregion

#pragma region Switch
protected:
	/// <summary>
	/// Update switching state
	/// </summary>
	/// <param name="bSwitch">if true, skil will switch next when skill ending</param>
	/// <param name="LineIndex">if this value less than 0, line will not switch</param>
	/// <param name="skilName">skill name, if it's invalid skill will not swich successfuly</param>
	void UpdateSwitchingState(bool bSwitch, int LineIndex, const FString& skilName);
	void ResetSwitchingState();
public:
	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "skillName"))
		bool SwitchSkill(const FString& lineName, const FString& skillName, bool bForce = false, bool bAutoPlay = false);
	bool SwitchSkill(int lineIndex, const FString& skillName);

	// for notify calling
	void OnSwitchEnd();
#pragma endregion

#pragma region switch conditions
	UFUNCTION(BlueprintCallable)
		bool IsExecuting();

	UFUNCTION(BlueprintCallable)
		bool CanSwitchFrom(const TArray<FString>& limitedList, bool fromAnySkill = false);

	UFUNCTION(BlueprintCallable)
		bool IsCurrentSkillLine(const FString& lineName);
	
	UFUNCTION(BlueprintCallable)
		bool IsAutoPlaying();

	// It's used in the hold line of skillComponent, skillLine and skill at run time
	ASkillDynamicData* GetDynamicData()
	{
		if (m_Data.DynamicData == nullptr)
		{
			m_Data.DynamicData = NewObject<ASkillDynamicData>();
		}
		return m_Data.DynamicData;
	}
#pragma endregion

#pragma region Loop Skill

	void OnStartLoopingSkill(const FString& loopSection, const FString& endSection, bool jumpEndOfSection)
	{
		GetDynamicData()->IsLooping = true;
		GetDynamicData()->LoopingSection = loopSection;
		GetDynamicData()->EndSection = endSection;
		GetDynamicData()->EndSectionJump = jumpEndOfSection;
	}

	void SetSkillLoopState(bool state)
	{
		GetDynamicData()->IsLooping = state;
	}

	UFUNCTION(BlueprintCallable)
	bool StopLooping();

#pragma endregion


#pragma region End skill
	UFUNCTION(BlueprintCallable)
		void StopSkill(bool bRule, const FAlphaBlend& InBlendOut);

	OnSkillEndDelegate OnSkllEndEvent;

	// For notify
	UFUNCTION(BlueprintImplementableEvent)
		void OnSkillEnded(const FString& SkillLineName);
#pragma endregion

protected:
	void OnSkillUpdate(ESkillLineState lineState);

};