#include "RemoteDetection/RemoteDetectorComponent.h"
#include "RemoteDetection/DetectRemote.h"

#include "MeleeDetection/CollisionDetect.h"
#include "MeleeDetection/RayDetect.h"

void URemoteDetectorComponent::InitData_Implementation(const FDataTableRowHandle &table, const FVector &direction, const FOnRemoteHit &callback)
{
    m_collisionDetector = NewObject<UCollisionDetect>(this, UCollisionDetect::StaticClass());
    if (m_collisionDetector)
    {

        auto infoPtr = table.GetRow<FDetectorInfo>("Find Collision Detect Info");

        if (infoPtr)
        {
            m_Hurt = infoPtr->Hurt;
            m_collisionDetector->Init(infoPtr->CollisionInfoTable);
            m_collisionDetector->SetDebug(m_bDebug);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Remote detector info is not correct!"))
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Collistion detector cannot be created!"))
    }

    m_Direction = direction;
    m_HitDelegate = callback;
}

void URemoteDetectorComponent::OnHit(const FDetectInfo &detectInfo)
{
    m_HitDelegate.ExecuteIfBound(detectInfo, m_Hurt);
}

bool URemoteDetectorComponent::IsFinished_Implementation()
{
    return m_bFinish;
}

void URemoteDetectorComponent::StartDetection_Implementation()
{
    m_bFinish = false;

    FCollisionDetectData collisionData;
    collisionData.Scale = GetComponentScale().X;
    collisionData.Size = m_Size;
    m_collisionDetector->UpdateData(collisionData);
}

void URemoteDetectorComponent::StopDetection_Implementation()
{
    m_bFinish = true;
    if (m_collisionDetector)
    {
        m_collisionDetector->OnEndDetection();
    }
}

void URemoteDetectorComponent::UpdateShape_Implementation(ECollisionDetectType shape)
{
    FCollisionDetectData collisionData;
    collisionData.Type = shape;
    m_collisionDetector->UpdateData(collisionData);
}

void URemoteDetectorComponent::UpdateSize_Implementation(FVector size)
{
    FCollisionDetectData collisionData;
    collisionData.Size = size;
    m_collisionDetector->UpdateData(collisionData);
}

void URemoteDetectorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    if (m_bFinish)
    {
        return;
    }

    TArray<FDetectInfo> detectResult;

    // Check collision
    if (m_collisionDetector)
    {
        FCollisionDetectData collisionData;
        collisionData.Scale = GetComponentScale().X;
        m_collisionDetector->UpdateData(collisionData);

        TArray<FDetectInfo> _collisionResult;
        m_collisionDetector->Detect(GetOwner(), GetComponentLocation(), GetComponentLocation(), _collisionResult, true);
        if (_collisionResult.Num() > 0)
        {
            detectResult.Append(_collisionResult);
        }
    }

    // TODO check ray

    // detect result
    if (detectResult.Num() > 0)
    {
        for (int i = 0; i < detectResult.Num(); ++i)
        {
            OnHit(detectResult[i]);
        }

        IRemoteDetector::Execute_PlayHitEffect(this);

        if (m_DestroyWhenHurt)
        {
            IRemoteDetector::Execute_StopDetection(this);
            DestroyComponent();
        }
    }

    // check life time
    if (m_LifeDuration > 0)
    {
        if (m_LifeBuffer > m_LifeDuration)
        {
            IRemoteDetector::Execute_StopDetection(this);
            DestroyComponent();
        }
        else
        {
            m_LifeBuffer += DeltaTime;
        }
        
    }
    
}