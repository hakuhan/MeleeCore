// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillComponent/SkillSwitchNotify.h"


void USkillSwitchNotify::NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration)
{
    if (MeshComp == nullptr || MeshComp->GetOwner() == nullptr)
    {
        return;
    }

    auto skillComponent = dynamic_cast<USkillComponent*>(MeshComp->GetOwner()->GetComponentByClass(USkillComponent::StaticClass()));
    if (skillComponent)
    {
        auto skillDynamicData = skillComponent->GetDynamicData();

        if (skillDynamicData)
        {
            skillDynamicData->IsSwitchable = true;
        }
    }
}

void USkillSwitchNotify::NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation)
{
    if (MeshComp == nullptr || MeshComp->GetOwner() == nullptr)
    {
        return;
    }

    auto skillComponent = dynamic_cast<USkillComponent*>(MeshComp->GetOwner()->GetComponentByClass(USkillComponent::StaticClass()));
    if (skillComponent)
    {
        auto skillDynamicData = skillComponent->GetDynamicData();

        if (skillDynamicData)
        {
            skillDynamicData->IsSwitchable = false;
        }
        skillComponent->OnSwitchEnd();
    }
}