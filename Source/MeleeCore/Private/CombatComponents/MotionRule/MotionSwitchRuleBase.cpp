#include "MotionRule/MotionSwitchRuleBase.h"

void UMotionSwitchRuleBase::UpdateAllMotions(const TArray<int>& switchMotions, const TArray<int>& forbiddenMotions)
{
    m_SwitchList.Empty();
    m_SwitchList.Append(switchMotions);
    m_ForbiddenList.Empty();
    m_ForbiddenList.Append(forbiddenMotions);
}

bool UMotionSwitchRuleBase::AddMotion(int id)
{
    bool result = false;

    if (!m_SwitchList.Contains(id))
    {
        m_SwitchList.Add(id);
        result = true;
    }

    return result;
}

void UMotionSwitchRuleBase::RemoveMotion(int id)
{
    if (m_SwitchList.Contains(id))
    {
        m_SwitchList.Remove(id);
    }
}

void UMotionSwitchRuleBase::UpdateAlternativeRules(const TMap<int, FMotionRuleEvent>& rules)
{
    m_AlternativeRules.Empty();

    for (auto _rule : rules)
    {
        m_AlternativeRules.Add(_rule.Key, _rule.Value.Callback);
    }
}

void UMotionSwitchRuleBase::UpdateBasicRule(int id, const FMotionRuleDelegate& event)
{
    if (m_BasicRules.Contains(id))
    {
        m_BasicRules[id] = event;
    }
    else
    {
        m_BasicRules.Add(id, event);
    }
}

void UMotionSwitchRuleBase::RemoveBasicRule(int id)
{
    if (m_BasicRules.Contains(id))
    {
        m_BasicRules.Remove(id);
    }
}

FMotionCheckResult UMotionSwitchRuleBase::IsSwitchable_Implementation(int id)
{
    FMotionCheckResult result;

    bool isContains = false;
    if (m_CurrentMotion == m_None)
    {
        isContains = false;
    }
    else if (m_CurrentMotion == m_All)
    {
        isContains = true;
    }
    else if (m_SwitchSelf && m_CurrentMotion == id)
    {
        isContains = true;
    }
    else
    {
        isContains = CheckMotion(id);
    }

    if (isContains)
    {
        result.bSwitchable = true;
        CheckRule(m_CurrentMotion, result);
    }

    return result;
}

void UMotionSwitchRuleBase::SwitchMotion_Implementation(int id)
{
    if (m_Info.Contains(id))
    {
        m_SwitchList = m_Info[id].SwitchableMotionIds;
        m_ForbiddenList = m_Info[id].ForbiddenMotionIds;
        m_SwitchSelf = m_Info[id].SwitchSelf;
    }

    m_CurrentMotion = id;
}

void UMotionSwitchRuleBase::EndMotion_Implementation(int id)
{
    if (id == m_CurrentMotion || id == m_None)
    {
        m_CurrentMotion = m_All;
    }
}

bool UMotionSwitchRuleBase::CheckMotion(int id)
{
    if (m_ForbiddenList.Contains(id))
    {
        return false;
    }
    return m_SwitchList.Contains(m_All) || m_SwitchList.Contains(id);
}

// If rule is not fonded, return true
void UMotionSwitchRuleBase::CheckRule(int id, FMotionCheckResult& outResult)
{
    if (m_AlternativeRules.Contains(id))
    {
        outResult = m_AlternativeRules[id].Execute();
    }
    else if (m_BasicRules.Contains(id))
    {
        outResult = m_BasicRules[id].Execute();
    }
}