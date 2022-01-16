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

bool USkillComponent::SwitchSkill(const FString& lineName, const FString& skillName, bool bForce, bool bAutoPlay)
{
    bool result = false;
    int index = m_Info.SkillLines.IndexOfByPredicate([&](const FSkillLineInfo& _line){
        return _line.Name == lineName;
    });

    if (index < 0)
    {
        UE_LOG(LogTemp, Error, TEXT("Cannot find target line!"))
        return false;
    }

    if (bForce || !m_LineControl->IsExecuting())
    {
        SwitchSkill(index, skillName);
        result = true;
    }
    else if (m_LineControl->CanSwitch())
    {
        UpdateSwitchingState(true, index, skillName);
        result = true;
    }

    m_Data.bAutoPlay = bAutoPlay;

    return result;
}


bool USkillComponent::SwitchSkill(int lineIndex, const FString& skillName)
{
    bool result = false;
    m_LineControl->OnSwitchOut();

    if (lineIndex >= 0 && m_Data.LineIndex != lineIndex)
    {
        m_LineControl->UpdateInfo(m_Info.SkillLines[lineIndex], GetOwner(), m_Data.DynamicData);
        m_Data.LineIndex = lineIndex;
    }

    if (skillName.IsEmpty())
    {
        result = m_LineControl->StartLine(skillName);
    }
    else
    {
        result = m_LineControl->SwitchSkill(skillName);
    }
    ResetSwitchingState();

    return result;
}

void USkillComponent::OnSwitchEnd()
{
    if (m_Data.bSwitch)
    {
        SwitchSkill(m_Data.SwitchLineIndex, m_Data.SwitchSkillName);
    }
    else if (m_Data.bAutoPlay)
    {
        bool executingSkill = ExecuteSkill();
        if (executingSkill)
        {
            SwitchSkill(m_Data.SwitchLineIndex, m_Data.SwitchSkillName);
        }
        else
        {
            m_Data.bAutoPlay = false;
        }
    }
}

bool USkillComponent::ExecuteSkill()
{
    bool result = false;

    switch (m_Data.State)
    {
        case ESkillState::SKILL_UNSTART:
        case ESkillState::SKILL_TERMINAl:
            m_LineControl->StartLine(FString());
            ResetSwitchingState();
            result = true;
            break;
        
        case ESkillState::SKILL_EXECUTING:
        {
            FString _nextSkillName = "";
            if (m_LineControl->CanSwitch() && m_LineControl->GetNextSkillName(_nextSkillName))
            {
                UpdateSwitchingState(true, -1, _nextSkillName);
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

void USkillComponent::UpdateSwitchingState(bool bSwitch, int LineIndex, const FString& skillName)
{
    m_Data.bSwitch = bSwitch;
    m_Data.SwitchLineIndex = LineIndex;
    m_Data.SwitchSkillName = skillName;
}


void USkillComponent::ResetSwitchingState()
{
    UpdateSwitchingState(false, -1, "");
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

bool USkillComponent::IsAutoPlaying() 
{
    return m_Data.bAutoPlay;    
}
