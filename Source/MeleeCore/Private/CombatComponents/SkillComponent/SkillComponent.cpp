#include "SkillComponent/SkillComponent.h"

void USkillComponent::BeginPlay()
{
    Super::BeginPlay();
    
    InitInfo();

    m_Data = FSkillComponentData(m_debug);

    m_LineControl = NewObject<USkillLine>();
    if (m_LineControl)
    {
        m_LineControl->OnChangeState.BindUObject(this, &USkillComponent::OnSkillUpdate);
        if (m_Info.SkillLines.Num() > 0)
        {
            m_LineControl->UpdateInfo(m_Info.SkillLines[0], GetOwner(), m_Data.DynamicData);
            m_Data.LineIndex = 0;
        }

        m_LineControl->OnSkillLineEnd.AddUObject(this, &USkillComponent::OnSkillEnded);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Line control haven't initialize"))
    }
}

bool USkillComponent::SwitchSkill(const FString& lineName, const FString& skillName, bool bForce)
{
    bool result = false;
    int index = m_Info.SkillLines.IndexOfByPredicate([&](const FSkillLineInfo& _line){
        return _line.Name == lineName;
    });

    if (bForce || !m_LineControl->IsExecuting())
    {
        SwitchSkill(index, skillName);
        ResetSwitchState();
        result = true;
    }
    else if (m_LineControl->CanSwitch())
    {
        UpdateSwitchState(true, index, skillName);
        result = true;
    }

    return result;
}


bool USkillComponent::SwitchSkill(int lineIndex, const FString& skillName)
{
    m_LineControl->OnSwitchOut();

    if (lineIndex >= 0 && m_Data.LineIndex != lineIndex)
    {
        m_LineControl->UpdateInfo(m_Info.SkillLines[lineIndex], GetOwner(), m_Data.DynamicData);
        m_Data.LineIndex = lineIndex;
    }

    return m_LineControl->StartLine(skillName);
}

void USkillComponent::OnSwitchEnd()
{
    if (m_Data.bSwitch)
    {
        SwitchSkill(m_Data.SwitchLineIndex, m_Data.SwitchSkillName);
    }

    ResetSwitchState();
    FSkillInfo skill;
}

bool USkillComponent::ExecuteSkill()
{
    bool result = false;

    switch (m_Data.State)
    {
        case ESkillState::SKILL_UNSTART:
        case ESkillState::SKILL_TERMINAl:
            m_LineControl->StartLine(FString());
            ResetSwitchState();
            result = true;
            break;
        
        case ESkillState::SKILL_EXECUTING:
        {
            FString _nextSkillName = "";
            if (m_LineControl->CanSwitch() && m_LineControl->GetNextSkillName(_nextSkillName))
            {
                UpdateSwitchState(true, -1, _nextSkillName);
                result = true;
            }
        }
        break;

        default:
            break;
    }

    return result;
}


bool USkillComponent::GetCurrentSkillName(FString& outName)
{
    if (IsExecuting())
    {
        return m_LineControl->GetCurrentSkillName(outName);
    }

    return false;
}

void USkillComponent::UpdateSwitchState(bool bSwitch, int LineIndex, const FString& skilName)
{
    m_Data.bSwitch = bSwitch;
    m_Data.SwitchLineIndex = LineIndex;
    m_Data.SwitchSkillName = skilName;
}


void USkillComponent::ResetSwitchState()
{
    UpdateSwitchState(false, 0, "");
}

bool USkillComponent::IsExecuting()
{
    return m_LineControl->IsExecuting();
}

bool USkillComponent::CanSwitchFrom(const TArray<FString>& limitedList, bool fromAnySkill)
{
    bool result = false;

    auto currentLineName = m_Info.SkillLines[m_Data.LineIndex].Name;

    if (limitedList.Contains(currentLineName) || fromAnySkill)
    {
        result = !m_LineControl->IsExecuting() || m_LineControl->CanSwitch();
    }

    return result;
}


bool USkillComponent::StopLooping()
{
    if (m_LineControl)
    {
        return m_LineControl->EndLooping();
    }

    return false;
}

void USkillComponent::StopSkill(bool bRule, const FAlphaBlend& InBlendOut)
{
    if (IsExecuting())
    {
        m_LineControl->FinishSkill(bRule, InBlendOut);
    }
}

void USkillComponent::OnSkillUpdate(ESkillLineState lineState)
{
    switch (lineState)
    {
    case ESkillLineState::SKILL_LINE_PLAYING:
        m_Data.State = ESkillState::SKILL_EXECUTING;
        break;

    case ESkillLineState::SKILL_LINE_FINISHED:
        m_Data.State = ESkillState::SKILL_TERMINAl;
        break;
    
    default:
        break;
    }
}

bool USkillComponent::IsCurrentSkillLine(const FString& lineName)
{
    bool result = false;

    if (!lineName.IsEmpty() && m_Info.SkillLines.IsValidIndex(m_Data.LineIndex))
    {
        result = m_Info.SkillLines[m_Data.LineIndex].Name == lineName;
    }

    return result;
}
