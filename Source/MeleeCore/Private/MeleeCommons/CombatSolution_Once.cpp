#include "CombatSolution_Once.h"

UCombatSolution_Once::~UCombatSolution_Once()
{
    m_Actors.Empty();
}

void UCombatSolution_Once::OnHit_Implementation(AActor* hitter, AActor *attackedActor, ECombatHitResult& outResult)
{
    if (!m_Actors.Contains(attackedActor))
    {
        m_Actors.Add(attackedActor);
        NoticeHit(hitter, attackedActor, outResult);
    }
}

void UCombatSolution_Once::InitData_Implementation()
{
}

void UCombatSolution_Once::OnStartDetection_Implementation()
{
    m_Actors.Empty();
}

void UCombatSolution_Once::OnEndDetection_Implementation()
{
}