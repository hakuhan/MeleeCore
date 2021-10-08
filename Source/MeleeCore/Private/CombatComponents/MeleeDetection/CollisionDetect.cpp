#include "MeleeDetection/CollisionDetect.h"

void UCollisionDetect::Init(FDataTableRowHandle table)
{
    auto infoPtr = table.GetRow<FCollisionDetectInfo>("Find Collision Detect Solution");
    if (infoPtr)
    {
        m_Info = *infoPtr;
    }
}

bool UCollisionDetect::Detect(AActor* const current, FVector curDetectPos, FVector preDetectPos, TArray<FDetectInfo>& outDetect, const bool IsCheckPerframe)
{
    bool result = false;
    // Set hitted data
    if (m_HittedInfos.Num() > 0)
    {
        outDetect.Empty();
        outDetect.Append(m_HittedInfos);
        m_HittedInfos.Empty();
        result = true;
    }

    // Spawn check
    CheckCollisionWhenSpawing();
    m_LastFrameSpawnedCollisions.Empty();
    
    // detect duration
    DestroyCheck();

    if (!IsCheckPerframe)
    {
        return result;
    }

    // Create detect obj by type
    auto _CollisionClass = m_Info.CollisionClassMaps[m_Data.Type];
    auto _CollisionObj = NewObject<UShapeComponent>(current, _CollisionClass);
    if (_CollisionObj)
    {
        // Add to actor
        current->AddInstanceComponent(_CollisionObj);
        _CollisionObj->RegisterComponent();

        // Base info
        _CollisionObj->SetVisibility(m_Info.IsShowCollision);
        _CollisionObj->SetHiddenInGame(!m_Info.IsShowCollision);
        _CollisionObj->ShapeColor = m_Data.Color;
        _CollisionObj->SetWorldLocation(curDetectPos);
        _CollisionObj->SetWorldRotation(current->GetActorRotation() + m_Data.Rotation);
        _CollisionObj->SetBoundsScale(m_Data.Scale);
        _CollisionObj->bShouldCollideWhenPlacing = true;

        // Setup shape
        switch (m_Data.Type)
        {
            case ECollisionDetectType::BOX_DETECT:
            {
                auto _BoxCollision = dynamic_cast<UBoxComponent*>(_CollisionObj);
                if (_BoxCollision)
                {
                    _BoxCollision->InitBoxExtent(m_Data.Size);
                }
            }
            break;

            case ECollisionDetectType::CAPSULE_DETECT:
            {
                auto _CapsuleCollision = dynamic_cast<UCapsuleComponent*>(_CollisionObj);
                if (_CapsuleCollision)
                {
                    _CapsuleCollision->InitCapsuleSize(m_Data.Size.X, m_Data.Size.Y);
                }
            }
            break;

            case ECollisionDetectType::SPHERE_DETECT:
            {
                auto _SphereCollision = dynamic_cast<USphereComponent*>(_CollisionObj);
                if (_SphereCollision)
                {
                    _SphereCollision->InitSphereRadius(m_Data.Size.X);
                }
            }
            break;

            default:

            break;
        }

        // Collision
        _CollisionObj->SetCollisionProfileName(m_Info.CollisionPresetName);
        _CollisionObj->OnComponentBeginOverlap.AddDynamic(this, &UCollisionDetect::OnOverlapBegin);
        
        m_CheckingCollisions.Add(_CollisionObj);
        m_LastFrameSpawnedCollisions.Add(_CollisionObj);
        m_DelayDestroyFrames.Add(1);
    }
        
    return result;
}

void UCollisionDetect::OnOverlapBegin(class UPrimitiveComponent* Comp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    FDetectInfo _detectInfo;
    _detectInfo.detectOwner = Comp->GetOwner();
    _detectInfo.target = OtherActor;
    _detectInfo.position = SweepResult.Location;
    m_HittedInfos.Add(_detectInfo);
}

void UCollisionDetect::OnEndDetection()
{
    m_HittedInfos.Empty();

    if (m_Info.IsShowCollision)
    {
        FTimerHandle _timer;
        GetWorld()->GetTimerManager().SetTimer(_timer, [&]{
            CleanCollision();
            GetWorld()->GetTimerManager().ClearTimer(_timer);
        }, 0.1f, false);
        return;
    }

    CleanCollision();
}

void UCollisionDetect::CleanCollision()
{
    for (int i = 0; i < m_CheckingCollisions.Num(); ++i)
    {
        DestroyCollision(m_CheckingCollisions[i]);
    }
    m_CheckingCollisions.Empty();
    m_DelayDestroyFrames.Empty();
    m_LastFrameSpawnedCollisions.Empty();
}

void UCollisionDetect::DestroyCheck()
{
    for (int i = m_CheckingCollisions.Num() - 1; i >= 0; --i)
    {
        if (m_CheckingCollisions[i] == nullptr || m_DelayDestroyFrames[i] >= m_Data.DurationFrame)
        {
            DestroyCollision(m_CheckingCollisions[i]);
            m_CheckingCollisions.RemoveAt(i);
            m_DelayDestroyFrames.RemoveAt(i);
        }
        else
        {
            ++m_DelayDestroyFrames[i];
        }
    }
    
}

void UCollisionDetect::CheckCollisionWhenSpawing()
{
    if (m_LastFrameSpawnedCollisions.Num() <= 0 )
    {
        return;
    }

    TArray<AActor*> _CollisionActors;
    for (int i = m_LastFrameSpawnedCollisions.Num() - 1; i >= 0; --i)
    {
        m_LastFrameSpawnedCollisions[i]->GetOverlappingActors(_CollisionActors);
        if (_CollisionActors.Num() > 0)
        {
            for (auto target : _CollisionActors)
            {
                FDetectInfo _detectInfo;
                _detectInfo.detectOwner = m_LastFrameSpawnedCollisions[i]->GetOwner();
                _detectInfo.target = target;
                _detectInfo.position = m_LastFrameSpawnedCollisions[i]->GetComponentLocation();
                m_HittedInfos.Add(_detectInfo);
            }
        }
    }
}

void UCollisionDetect::DestroyCollision(UShapeComponent* target)
{
    if (target)
    {
        target->DestroyComponent();
    }
}