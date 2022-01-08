#include "CombatSolution_Multi.h"

UCombatSolution_Multi::~UCombatSolution_Multi()
{
    m_Actors.Empty();
    m_IntervalTimers.Empty();
}

void UCombatSolution_Multi::InitData_Implementation()
{
    auto info = (FHurtMulti *)&m_HurtInfo;
    if (info != nullptr)
    {
        m_MeleeInterval = info->interval;
    }
}

void UCombatSolution_Multi::OnStartDetection_Implementation()
{
    m_Actors.Empty();
    m_IntervalTimers.Empty();
}

void UCombatSolution_Multi::OnHit_Implementation(AActor* hitter, AActor *actor, ECombatHitResult &outResult)
{
    if (!IsCooling(actor))
    {
        NoticeHit(hitter, actor, outResult);

        m_Actors.Add(actor);
        GetWorld()->GetTimerManager().SetTimer(m_IntervalTimers[actor], this, &UCombatSolution_Multi::CoolOver, m_MeleeInterval, false);
    }
}

void UCombatSolution_Multi::OnEndDetection_Implementation()
{
}

bool UCombatSolution_Multi::IsCooling(AActor *hittedActor)
{
    bool result = false;
    if (!m_IntervalTimers.Contains(hittedActor))
    {
        result = true;
        FTimerHandle timer;
        m_IntervalTimers.Add(hittedActor, timer);
    }
    else
    {
        result = GetWorld()->GetTimerManager().IsTimerActive(m_IntervalTimers[hittedActor]);
    }

    // UE_LOG(LogTemp, Warning, TEXT("Cool result %d"), result);

    return result;
}

void UCombatSolution_Multi::CoolOver()
{
    m_IntervalTimers.Empty();
    // UE_LOG(LogTemp, Warning, TEXT("Cool over"));
}

void UCombatSolution_Multi::UpdateHurts(FHurt data)
{
    ICombatSolution::UpdateHurts(data);
    auto info = (FHurtMulti *)&data;
    if (info)
    {
        m_MeleeInterval = info->interval;
    }
}