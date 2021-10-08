#include "SkillComponent/Skill.h"

void USkill::Tick(float DeltaTime)
{
    if (!m_DynamicData && !m_Data.IsEnable)
    {
        return;
    }

    // montage state
    auto montageInst = m_TargetAnim->GetActiveInstanceForMontage(m_Info.Montage);
    if (!m_Info.Montage || !m_TargetAnim || !montageInst)
    {
        m_Data.ExecuteState = ESkillExecuteState::SKILL_EXECUTE_END;
        m_Data.IsEnable = false;
    }
    else if (montageInst->IsPlaying())
    {
        m_Data.ExecuteState = ESkillExecuteState::SKILL_EXECUTE_WORK;
    
        if (m_DynamicData->IsSwitchable)
        {
            m_Data.ExecuteState = ESkillExecuteState::SKILL_EXECUTE_SWITCHABLE;
        }

        if (!m_Info.Loop)
        {
            m_Data.TimeLeft -= DeltaTime;

            // Stop
            if (m_Data.TimeLeft < 0)
            {
                Terminate();
            }
        }
    }
}

void USkill::UpdateData(const FSkillInfo& info, ASkillDynamicData* dynamicData)
{
    m_Info = info;
    m_Data = FSkillData();
    m_DynamicData = dynamicData;

}

void USkill::ExecuteSkill(AActor* target)
{
    if (USkeletalMeshComponent *mesh = target->FindComponentByClass<USkeletalMeshComponent>())
    {
        UAnimInstance *animInst = mesh->GetAnimInstance();
        if (animInst && m_Info.Montage != nullptr)
        {
            if (m_DynamicData->bDebug)
                UE_LOG(LogTemp, Warning, TEXT("Execute Skill: %s"), *(m_Info.Name));
            m_Data.TimeLeft = animInst->Montage_Play(m_Info.Montage);
            m_TargetAnim = animInst;
            m_Data.IsEnable = true;
            m_Data.ExecuteState = ESkillExecuteState::SKILL_EXECUTE_WORK;
        }
    }
}


bool USkill::IsMatch(const FSkillInfo& info)
{
    return m_Info == info;   
}

bool USkill::CanSwitch()
{
    return m_Data.ExecuteState == ESkillExecuteState::SKILL_EXECUTE_SWITCHABLE;
}

void USkill::Terminate()
{
    if (!m_TargetAnim)
    {
        return;
    }

    auto montageInst = m_TargetAnim->GetActiveInstanceForMontage(m_Info.Montage);
    if (montageInst)
    {
        montageInst->Terminate();
        if (m_DynamicData->bDebug)
            UE_LOG(LogTemp, Warning, TEXT("Terminate Skill: %s"), *(m_Info.Name));
    }

    m_Data.ExecuteState = ESkillExecuteState::SKILL_EXECUTE_END;
    m_Data.IsEnable = false;
}

void USkill::Stop(const FAlphaBlend& InBlendOut)
{
    if (!m_TargetAnim)
    {
        return;
    }

    auto montageInst = m_TargetAnim->GetActiveInstanceForMontage(m_Info.Montage);
    if (montageInst)
    {
        montageInst->Stop(InBlendOut);
        if (m_DynamicData->bDebug)
            UE_LOG(LogTemp, Warning, TEXT("Stop Skill: %s"), *(m_Info.Name));
    }

    m_Data.ExecuteState = ESkillExecuteState::SKILL_EXECUTE_END;
    m_Data.IsEnable = false;
}

bool USkill::EndLooping()
{
    if (m_DynamicData && m_DynamicData->IsLooping && m_Data.IsEnable && m_TargetAnim)
    {
        m_TargetAnim->Montage_JumpToSection(FName(*m_DynamicData->EndSection), m_Info.Montage);
        return true;
    }

    return false;
}
