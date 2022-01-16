#include "SkillComponent/SkillLine.h"

void USkillLine::Tick(float DeltaTime)
{
    if (!m_DynamicData || !m_Data.IsEnable)
    {
        return;
    }

    if (m_Skill->GetState() == ESkillExecuteState::SKILL_EXECUTE_END)
    {
        UpdateState(ESkillLineState::SKILL_LINE_FINISHED);
        m_Data.IsEnable = false;
        OnSkillLineEnd.Broadcast(m_Info.Name);
    }
}

void USkillLine::UpdateInfo(const FSkillLineInfo &info, AActor* target, ASkillDynamicData* dynamicData)
{
    m_Info = info;
    m_Target = target;
    m_DynamicData = dynamicData;
}

bool USkillLine::SwitchSkill(const FString& Name)
{
    int index = m_Info.SkillLine.IndexOfByPredicate([&](const FSkillInfo& _info){
        return _info.Name == Name;
    });

    if (index >= 0)
    {
        return SwitchSkillByIndex(index);
    }
    
    return false;
}

bool USkillLine::StartLine(const FString& skillName)
{
    bool result = false;
    if (skillName.IsEmpty())
    {
        result = SwitchSkillByIndex(0);
        if (m_DynamicData->bDebug)
            UE_LOG(LogTemp, Warning, TEXT("Begin Skill line: %s"), *(m_Info.Name));
    }
    else
    {
        result = SwitchSkill(skillName);
    }

    if (result)
    {
        m_Data.Record.SkillLine.Empty();
        UpdateState(ESkillLineState::SKILL_LINE_PLAYING);
        m_Data.IsEnable = true;
    }

    return result;
}


bool USkillLine::GetCurrentSkillName(FString& outName)
{
    if (m_Info.SkillLine.IsValidIndex(m_Data.SkillOffset))
    {
        outName = m_Info.SkillLine[m_Data.SkillOffset].Name;
        return true;
    }

    return false;
}

bool USkillLine::GetNextSkillName(FString& outName)
{
    if (m_Info.SwitchType == ELineSwitchType::LINE_SEQUENCE && m_Info.SkillLine.IsValidIndex(m_Data.SkillOffset + 1))
    {
        outName = m_Info.SkillLine[m_Data.SkillOffset + 1].Name;
        return true;
    }

    return false;
}

bool USkillLine::IsExecuting()
{
    return m_Data.State == ESkillLineState::SKILL_LINE_PLAYING;
}

bool USkillLine::IsEndSkillLine()
{
    return m_Data.State == ESkillLineState::SKILL_LINE_FINISHED;
}

void USkillLine::FinishSkill(bool terminate, const FAlphaBlend& InBlendOut)
{
    if (m_Skill)
    {
        if (terminate)
        {
            m_Skill->Terminate();
        }
        else
        {
            m_Skill->Stop(InBlendOut);
        }
    }

    UpdateState(ESkillLineState::SKILL_LINE_FINISHED);
}

bool USkillLine::CanSwitch()
{
    bool result = false;
    if (m_Skill)
    {
        result = m_Skill->CanSwitch();
    }

    return result;
}


bool USkillLine::EndLooping()
{
    if (m_Skill)
    {
        return m_Skill->EndLooping();
    }

    return false;
}

bool USkillLine::SwitchSkillByIndex(int index)
{
    if (m_Info.SkillLine.IsValidIndex(index))
    {
        if (UpdateSkill(m_Info.SkillLine[index]))
        {
            m_Skill->ExecuteSkill(m_Target);
        }
        
        m_Data.SkillOffset = index;
        m_Data.Record.SkillLine.Add(m_Info.SkillLine[index]);
        return true;
    }

    return false;
}

void USkillLine::OnSwitchOut()
{
    UpdateState(ESkillLineState::SKILL_LINE_JUMPED);
}

void USkillLine::GetSkillRecord(FSkillLineInfo& records)
{
    records = m_Data.Record;
}

bool USkillLine::IsCurrentSkillOver()
{
    bool result = false;
    if (m_Data.State != ESkillLineState::SKILL_LINE_UNSTART)
    {
        if (m_Skill)
        {
            result = m_Skill->m_Data.ExecuteState == ESkillExecuteState::SKILL_EXECUTE_END;
        }
    }

    return result;
}

USkill* USkillLine::UpdateSkill(const FSkillInfo& skillInfo)
{
    if (m_Skill == nullptr)
    {
        m_Skill = NewObject<USkill>();
    }

    m_Skill->UpdateData(skillInfo, m_DynamicData);

    return m_Skill;
}

bool USkillLine::GetCurrentSkillInfo(FSkillInfo& outInfo)
{
    bool result = false;

    if (m_Info.SkillLine.IsValidIndex(m_Data.SkillOffset))
    {
        outInfo = m_Info.SkillLine[m_Data.SkillOffset];
        result = true;
    }

    return result;
}

void USkillLine::UpdateState(ESkillLineState state)
{
    m_Data.State = state;
    OnChangeState.ExecuteIfBound(state);
}

