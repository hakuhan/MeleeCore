#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "SkillComponent.h"
#include "AnimLoopState.generated.h"

/**
 *
 */
UCLASS()
class MELEECORE_API UAnimLoopState : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	USkillComponent* m_Skill;
	UPROPERTY(EditAnywhere, Category = "Skill")
	FString LoopSectionName;
	UPROPERTY(EditAnywhere, Category = "Skill")
	FString EndSectionName;
	UPROPERTY(EditAnywhere, Category = "Skill")
	bool JumpEndOfSection;

public:
	void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
	{
		m_Skill = dynamic_cast<USkillComponent*>(MeshComp->GetOwner()->GetComponentByClass(USkillComponent::StaticClass()));
		if (m_Skill)
		{
			m_Skill->OnStartLoopingSkill(LoopSectionName, EndSectionName, JumpEndOfSection);
		}
	}

	void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
	{
		if (m_Skill)
		{
			m_Skill->SetSkillLoopState(true);
		}
	}

	void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
	{
		if (m_Skill)
		{
			m_Skill->SetSkillLoopState(true);
		}
	}
};
