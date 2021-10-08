#include "MeleeDetection/MeleeDetectionNotify.h"

UMeleeDetectionNotify::UMeleeDetectionNotify()
{
    auto curveObj = ConstructorHelpers::FObjectFinder<UCurveFloat>(TEXT("/MeleeCore/Curves/C_DefaultCombat.C_DefaultCombat"));
    if (curveObj.Object != nullptr)
    {
        m_HurtCurve = curveObj.Object;
    }
}

void UMeleeDetectionNotify::BeginDestroy()
{
    Super::BeginDestroy();

    m_MeleeCombat = nullptr;
    m_HurtCurve = nullptr;
}

void UMeleeDetectionNotify::NotifyBegin(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *Animation, float TotalDuration)
{
    if (MeshComp == nullptr || MeshComp->GetOwner() == nullptr)
    {
        return;
    }

    m_TotalDuration = TotalDuration;
    m_TimeBuffer = 0;

    m_MeleeCombat = dynamic_cast<UDetectMelee*>(MeshComp->GetOwner()->GetComponentByClass(UDetectMelee::StaticClass()));

    if (m_MeleeCombat != nullptr && m_HurtCurve != nullptr)
    {
        m_MeleeCombat->UpdateDetect(m_DetectType);
        m_MeleeCombat->UpdateCollisionData(m_CollisionData);
        m_MeleeCombat->UpdateHurts(m_Hurt, m_Solution);
        m_MeleeCombat->UpdateHurtRate(m_HurtCurve->GetFloatValue(0));
        m_MeleeCombat->UpdateWeaponMask(m_WeaponFlags);
        
        m_MeleeCombat->StartDetection(TotalDuration);
    }
}

void UMeleeDetectionNotify::NotifyTick(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *Animation, float FrameDeltaTime)
{
    if (m_MeleeCombat != nullptr && m_HurtCurve != nullptr)
    {
        m_TimeBuffer += FrameDeltaTime;
        float currentRate = m_HurtCurve->GetFloatValue(m_TimeBuffer / m_TotalDuration);
        m_MeleeCombat->UpdateHurtRate(currentRate);
    }
}

void UMeleeDetectionNotify::NotifyEnd(USkeletalMeshComponent *MeshComp, UAnimSequenceBase *Animation)
{
    if (m_MeleeCombat != nullptr)
    {
        m_MeleeCombat->EndDetection();
        m_MeleeCombat->ResetHurts();
    }
}