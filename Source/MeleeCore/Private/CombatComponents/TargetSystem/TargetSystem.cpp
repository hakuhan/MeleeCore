#include "TargetSystem/TargetSystem.h"
#include "Components/SphereComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"

UTargetSystem::UTargetSystem()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UTargetSystem::BeginPlay()
{
    Super::BeginPlay();

    if (!m_IsUpdatedInfo && !m_InfoTable.IsNull())
    {
        m_IsUpdatedInfo = true;

        auto targetInfoPtr = m_InfoTable.GetRow<FTargetInfo>("Find target info");
        m_Info = *targetInfoPtr;
        m_Data.Reset();
    }

    if (m_Info.CameraSpeedCurve == nullptr)
    {
        m_Info.CameraSpeedCurve = NewObject<UCurveFloat>();
        m_Info.CameraSpeedCurve->FloatCurve.AddKey(0, 0);
        m_Info.CameraSpeedCurve->FloatCurve.AddKey(1, 1);
        m_Info.CameraCurveXDuration = 1;
        m_Info.CameraCurveYDuration = 1;
    }
}

void UTargetSystem::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!m_Data.bAnimating && !m_Data.bLocking)
    {
        return;
    }

    // Rotate component
    USceneComponent *rotateComponent = nullptr;

    auto springArm = GetOwner()->GetComponentByClass(USpringArmComponent::StaticClass());

    // Find correct camera rotation
    if (springArm)
    {
        rotateComponent = dynamic_cast<USceneComponent *>(springArm);
    }
    else
    {
        auto camera = GetOwner()->GetComponentByClass(UCameraComponent::StaticClass());
        if (camera)
        {
            rotateComponent = dynamic_cast<USceneComponent *>(camera);
        }
    }

    // Rotate Animation
    if (m_Data.bAnimating)
    {
        // Get curve value
        float lerpValue = FMath::Lerp(0.0f, m_Info.CameraCurveXDuration, m_Data.AnimTimeBuffer / m_Info.SwitchCameraDuration * m_Info.CameraCurveXDuration);

        // Get lerp value of roation
        FRotator _rotator = UKismetMathLibrary::RLerp(m_Data.CameraStartRotator, m_Data.CameraTargetRotator, lerpValue, true);

        if (rotateComponent)
        {
            rotateComponent->SetRelativeRotation(_rotator);
        }
        else
        {
            m_Data.bAnimating = false;
            UE_LOG(LogTemp, Error, TEXT("camera component is not valid!"))
        }

        if (m_Data.AnimTimeBuffer > m_Info.SwitchCameraDuration)
        {
            m_Data.bAnimating = false;
        }
        m_Data.AnimTimeBuffer += DeltaTime;
    }
    // Lock Target
    else if (m_Data.bLocking)
    {
        if (m_Data.CurrentTarget && rotateComponent)
        {
            // Get direction
            auto _direction = m_Data.CurrentTarget->GetActorLocation() - GetOwner()->GetActorLocation();

            // Get rotator
            m_Data.CameraTargetRotator = _direction.ToOrientationRotator();
            m_Data.CameraTargetRotator.Pitch = rotateComponent->GetRelativeRotation().Pitch;
            rotateComponent->SetRelativeRotation(m_Data.CameraTargetRotator);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("target or camera not valid!"))
            m_Data.bLocking = false;
        }
    }
}

void UTargetSystem::UpdateInfo(const FTargetInfo &info)
{
    m_IsUpdatedInfo = true;
    m_Info = info;
    m_Data.Reset();
}

void UTargetSystem::SwitchTarget(bool useDefaultSetting, ESwitchToRule newRule, float newDetectRadius)
{
    if (m_CheckingComponent && m_CheckingComponent->IsActive())
    {
        return;
    }

    if (!m_IsUpdatedInfo)
    {
        UE_LOG(LogTemp, Warning, TEXT("Target system info hasn't setted! It uses default setting"))
    }

    if (useDefaultSetting)
    {
        m_Data.SwitchingRule = m_Info.SwitchingRule;
        m_Data.DetectRadius = m_Info.DetectRadius;
    }
    else
    {
        m_Data.SwitchingRule = newRule;
        m_Data.DetectRadius = newDetectRadius;
    }

    m_Data.AvailableTargets.Empty();

    // Find target by range
    if (!m_CheckingComponent)
    {
        m_CheckingComponent = NewObject<USphereComponent>(this, USphereComponent::StaticClass());
        // Base info
        m_CheckingComponent->SetVisibility(m_bDebug);
        m_CheckingComponent->SetHiddenInGame(!m_bDebug);
        if (m_bDebug)
        {
            m_CheckingComponent->ShapeColor = FColor::Green;
        }

        m_CheckingComponent->SetWorldLocation(GetOwner()->GetActorLocation());
        m_CheckingComponent->bShouldCollideWhenPlacing = true;
        m_CheckingComponent->InitSphereRadius(m_Data.DetectRadius * 100);
        m_CheckingComponent->SetCollisionProfileName(m_Info.CollisionPresetName);
        m_CheckingComponent->OnComponentBeginOverlap.AddDynamic(this, &UTargetSystem::OnOverlabBegin);
        m_CheckingComponent->RegisterComponent();
        m_CheckingComponent->AttachToComponent(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
    }
    else
    {
        m_CheckingComponent->SetActive(true);
        m_CheckingComponent->SetSphereRadius(m_Data.DetectRadius * 100);
    }

    FTimerHandle handle;
    GetOwner()->GetWorld()->GetTimerManager().SetTimer(
        handle, [&]
        {
            m_CheckingComponent->SetActive(false);
            m_CheckingComponent->SetSphereRadius(0);

            // Find best target
            AActor *target = FindBestTarget(m_Data.AvailableTargets);
            if (target)
            {
                m_Data.CurrentTarget = target;
                m_OnSeleteTarget.Broadcast(GetOwner(), target);
                if (m_bDebug)
                {
                    UE_LOG(LogTemp, Error, TEXT("Best target is: %s"), *(target->GetActorLabel()))
                }

                // Switch target behavior
                switch (m_Data.SwitchingRule)
                {
                case ESwitchToRule::TARGET_LockAndFaceTo:
                    FaceToTarget();
                case ESwitchToRule::TARGET_Lock:
                    LockOnTarget();
                    break;

                case ESwitchToRule::TARGET_FaceTo:
                    FaceToTarget();
                    break;

                case ESwitchToRule::TARGET_LookAtAndFaceTo:
                    FaceToTarget();
                case ESwitchToRule::TARGET_LookAt:
                    LookAtTarget();
                    break;
                default:
                    UE_LOG(LogTemp, Error, TEXT("Rule not implemented!"))
                    break;
                }
            }
        },
        m_Info.m_CheckingDuration, false);
}

AActor *UTargetSystem::FindBestTarget_Implementation(const TArray<AActor *> &avaliables)
{
    if (avaliables.Num() == 0)
    {
        return nullptr;
    }

    struct AvaliableTargetInfo
    {
        AActor *Target;
        FVector Position;
        float Angle;
        float RelativeDistance;
        float RelativeHight;
    };

    // Checking range lambda
    auto CheckTargetRangeLambda = [&](float _pointAngle, float _range, float _maxDistance, float _hightRange, const AvaliableTargetInfo &_targetInfo)
    {
        if (_targetInfo.RelativeHight > _hightRange)
        {
            return false;
        }

        // check with distance
        if (_targetInfo.RelativeDistance > _maxDistance)
        {
            return false;
        }

        float _minAngle = _pointAngle - _range;
        float _maxAngle = _pointAngle + _range;
        // if coss 180 digree. condition must reverse because range is from -180 to 180
        bool _cross = false;
        if (_minAngle < -180)
        {
            _cross = true;
            _minAngle = _maxAngle;
            _maxAngle = (int)(-_pointAngle + _range) % 180;
        }
        else if (_maxAngle > 180)
        {
            _cross = true;
            _maxAngle = _minAngle;
            _minAngle = ((int)(_pointAngle + _range) % 180) * -1;
        }

        bool _withinRange = false;
        if (_cross)
        {
            if (_targetInfo.Angle < _minAngle && _targetInfo.Angle > _maxAngle)
            {
                _withinRange = true;
            }
        }
        else if (_targetInfo.Angle > _minAngle && _targetInfo.Angle <= _maxAngle)
        {
            _withinRange = true;
        }

        return _withinRange;
    };

    // Find best rule
    auto CompareBestTargetLambda = [&](const AvaliableTargetInfo &newTargetInfo, const AvaliableTargetInfo &curTargetInfo, float targetAnglePoint, EBestFindingTagetRule rule)
    {
        bool _result = false;
        // check conditions
        float _preDistancePoint = 0;
        float _curDistancePoint = 0;
        float _preAnglePoint = 0;
        float _curAnglePoint = 0;
        if (rule != EBestFindingTagetRule::FIND_DistanceFirst && rule != EBestFindingTagetRule::FIND_AngleFirst)
        {
            // Point high priority less
            _preDistancePoint = curTargetInfo.RelativeDistance * m_Info.DistanceWeight;
            _curDistancePoint = newTargetInfo.RelativeDistance * m_Info.DistanceWeight;
            _preAnglePoint = FMath::Abs(curTargetInfo.Angle - targetAnglePoint) * m_Info.AngleWeight;
            _curAnglePoint = FMath::Abs(newTargetInfo.Angle - targetAnglePoint) * m_Info.AngleWeight;
        }

        switch (rule)
        {
        case EBestFindingTagetRule::FIND_DistanceFirst:
            if (newTargetInfo.RelativeDistance < curTargetInfo.RelativeDistance)
            {
                _result = true;
            }
            break;

        case EBestFindingTagetRule::FIND_AngleFirst:
            if (FMath::Abs(newTargetInfo.Angle - targetAnglePoint) < FMath::Abs(curTargetInfo.Angle - targetAnglePoint))
            {
                _result = true;
            }
            break;

        case EBestFindingTagetRule::FIND_Banace:
            if ((_preDistancePoint + _preAnglePoint) > (_curDistancePoint + _curAnglePoint))
            {
                _result = true;
            }
            break;

        case EBestFindingTagetRule::FIND_DistanceGold:
            if ((_preDistancePoint * 0.618 + _preAnglePoint * 0.32) > (_curDistancePoint * 0.618 + _curAnglePoint * 0.32))
            {
                _result = true;
            }

            break;

        case EBestFindingTagetRule::FIND_AngleGold:
            if ((_preDistancePoint * 0.32 + _preAnglePoint * 0.618) > (_curDistancePoint * 0.32 + _curAnglePoint * 0.618))
            {
                _result = true;
            }
            break;

        default:
            UE_LOG(LogTemp, Error, TEXT("Find target rule type invalid!"))
            break;
        }

        return _result;
    };

    // Find all target's position and angle
    TArray<AvaliableTargetInfo> targetInfos;
    FVector forwardVector = GetOwner()->GetActorForwardVector();
    FVector ownerLocation = GetOwner()->GetActorLocation();
    for (auto _target : avaliables)
    {
        AvaliableTargetInfo _info;
        _info.Target = _target;
        _info.Position = _target->GetActorLocation();
        FVector _relationVector = _info.Position - ownerLocation;
        FVector _relativeVectorNormalize = _relationVector.GetSafeNormal();
        _info.Angle = FMath::RadiansToDegrees(acosf(FVector::DotProduct(forwardVector, _relativeVectorNormalize)));
        FVector _crossVector = FVector::CrossProduct(forwardVector, _relativeVectorNormalize);
        if (_crossVector.Z > 0)
        {
            _info.Angle *= -1;
        }
        _info.RelativeDistance = _relationVector.Size() / 100;
        _info.RelativeHight = (ownerLocation.Z - _info.Position.Z) / 100;

        targetInfos.Add(_info);
    }

    if (m_bDebug)
    {
        FString _allTargets;
        for (auto _info : targetInfos)
        {
            if (_info.Target)
            {
                _allTargets += _info.Target->GetActorLabel() + ",";
                _allTargets += "location:" + _info.Position.ToString() + ",";
                _allTargets += "Angle:" + FString::Printf(TEXT("%f"), _info.Angle) + ",";
                _allTargets += "Distance:" + FString::Printf(TEXT("%f"), _info.RelativeDistance) + ",";
                _allTargets += "Hight:" + FString::Printf(TEXT("%f"), _info.RelativeHight) + "\n";
            }
        }
        UE_LOG(LogTemp, Error, TEXT("All targets: %s"), *_allTargets)
    }

    // Check danger positions
    AActor *dangerousActor = nullptr;
    int dangerousIndex = 0;
    for (float _dangerPointAngle : m_Info.DangerousPoints)
    {
        for (int i = 0; i < targetInfos.Num(); ++i)
        {
            bool _isInRange = CheckTargetRangeLambda(_dangerPointAngle, m_Info.DangerousAngleRange, m_Info.DangerousDistance, m_Info.DangerousHightRange, targetInfos[i]);

            if (_isInRange && CompareBestTargetLambda(targetInfos[i], targetInfos[dangerousIndex], _dangerPointAngle, m_Info.BestTargetRule))
            {
                dangerousIndex = i;
                dangerousActor = targetInfos[i].Target;
            }
        }
    }
    if (dangerousActor)
    {
        return dangerousActor;
    }

    // Check Pre target
    if (m_Info.bPreTargetFirst && m_Data.CurrentTarget)
    {
        int _preTargetIndex = targetInfos.IndexOfByPredicate([&](const AvaliableTargetInfo &_target)
                                                             { return _target.Target == m_Data.CurrentTarget; });
        if (_preTargetIndex >= 0 && CheckTargetRangeLambda(0, m_Info.FocusRange, m_Info.FocusDistance, m_Info.DetectHightRange, targetInfos[_preTargetIndex]))
        {
            return targetInfos[_preTargetIndex].Target;
        }
    }

    // Check focus range
    AActor *focusingActor = nullptr;
    int focusIndex = 0;
    for (int i = 0; i < targetInfos.Num(); ++i)
    {
        bool _isInRange = CheckTargetRangeLambda(0, m_Info.FocusRange, m_Info.FocusDistance, m_Info.FocusHightRange, targetInfos[i]);
        if (_isInRange)
        {
            if (_isInRange && CompareBestTargetLambda(targetInfos[i], targetInfos[focusIndex], 0, m_Info.BestTargetRule))
            {
                focusIndex = i;
                focusingActor = targetInfos[i].Target;
            }
        }
    }
    if (focusingActor)
    {
        return focusingActor;
    }

    // Check Rest targets
    AActor *restBestActor = targetInfos[0].Target;
    int restIndex = 0;
    for (int i = 0; i < targetInfos.Num(); ++i)
    {
        if (CompareBestTargetLambda(targetInfos[i], targetInfos[restIndex], 0, m_Info.BestTargetRule))
        {
            restIndex = i;
            restBestActor = targetInfos[i].Target;
        }
    }

    return restBestActor;
}

void UTargetSystem::OnOverlabBegin(class UPrimitiveComponent *Comp, class AActor *OtherActor, class UPrimitiveComponent *OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
    bool isTarget = false;
    for (auto _tag : OtherActor->Tags)
    {
        if (m_Info.TargetTags.Contains(_tag))
        {
            isTarget = true;
            break;
        }
    }

    if (isTarget)
    {
        m_Data.AvailableTargets.Add(OtherActor);
    }
}

bool UTargetSystem::LockOnTarget_Implementation()
{
    if (!m_Data.bLocking)
    {
        m_Data.bLocking = true;
        return LookAtTarget();
    }

    return true;
}

bool UTargetSystem::LookAtTarget_Implementation()
{
    if (GetOwner() == nullptr || m_Data.CurrentTarget == nullptr)
    {
        return false;
    }

    // Check spring arm and camera
    auto camera = GetOwner()->GetComponentByClass(UCameraComponent::StaticClass());
    auto springArm = GetOwner()->GetComponentByClass(USpringArmComponent::StaticClass());
    if (camera == nullptr)
    {
        UE_LOG(LogTemp, Error, TEXT("Cannot find any spring arm or camera for rotation"))
        return false;
    }

    // Get rotate component
    USceneComponent *rotateComponent = nullptr;
    if (springArm)
    {
        rotateComponent = dynamic_cast<USceneComponent *>(springArm);
    }
    else if (camera)
    {
        rotateComponent = dynamic_cast<USceneComponent *>(camera);
    }

    if (rotateComponent)
    {
        // Start rotator
        m_Data.CameraStartRotator = rotateComponent->GetRelativeRotation();

        // Get direction
        auto _direction = m_Data.CurrentTarget->GetActorLocation() - GetOwner()->GetActorLocation();

        // Get target rotator
        m_Data.CameraTargetRotator = _direction.ToOrientationRotator();
        m_Data.CameraTargetRotator.Pitch = m_Data.CameraStartRotator.Pitch;

        m_Data.bAnimating = true;
        m_Data.AnimTimeBuffer = 0;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Cannot find any spring arm or camera for rotation"))
        return false;
    }

    return true;
}

bool UTargetSystem::FaceToTarget_Implementation()
{
    if (m_Data.CurrentTarget && GetOwner())
    {
        FVector direction = m_Data.CurrentTarget->GetActorLocation() - GetOwner()->GetActorLocation();
        FRotator rotator = direction.ToOrientationRotator();
        GetOwner()->SetActorRotation(rotator);
        return true;
    }

    return false;
}

void UTargetSystem::UnLockTarget_Implementation()
{
    m_Data.bLocking = false;
}

bool UTargetSystem::IsLocking()
{
    return m_Data.bLocking;
}