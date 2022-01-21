#include "MeleeUtils.h"
#include "MeleeDetection/DetectMelee.h"
#include "Animation/AnimMontage.h"

void UMeleeUtils::GetImplementFromActor(AActor *owner, TSubclassOf<UInterface> interface, TArray<UObject *> &array, bool checkSelf)
{
    if (owner == nullptr)
    {
        return;
    }

    // Check components
    auto comp = owner->GetComponentsByInterface(interface);
    for (int i = 0; i < comp.Num(); ++i)
    {
        array.Add(comp[i]);
    }

    // Check self
    if (checkSelf && owner->GetClass()->ImplementsInterface(interface))
    {
        array.Add(owner);
    }
}

bool UMeleeUtils::EnableWeaponByType(UDetectMelee *target, UPARAM(meta=(Bitmask, BitmaskEnum=EAttackWeapon)) uint8 WeaponType, bool enable, bool refreshWeapons)
{
    bool result = false;

    if (target == nullptr || target->GetOwner() == nullptr)
    {
        UE_LOG(LogTemp, Warning,TEXT("Delect component or actor does not valid!"));
        return result;
    }

    if (refreshWeapons)
    {
        target->UpdateWeapon();
    }

    // Change weapon state
    TArray<TScriptInterface<IMeleeWeapon>> weapons;
    target->GetWeapons(weapons);
    for (auto w : weapons)
    {
        if (w && w->IsTargetWeapon(WeaponType))
        {
            w->SetWeaponEnabled(enable);
            result = true;
        }
    }

    return result;
}

bool UMeleeUtils::AttachWeapon(USceneComponent* target, UPARAM(meta=(AllowAbstract = "UMeleeWeapon")) USceneComponent* weapon, const FString& socket)
{
    bool result = false;

    // check attached
    TArray<USceneComponent*> parents;
    weapon->GetParentComponents(parents);
    if (parents.FindByPredicate([&](USceneComponent* p){
        return p == target && weapon->GetAttachSocketName().ToString() == socket;
    }))
    {
        UE_LOG(LogTemp, Warning, TEXT("Weapon: %s aready attached!"), *weapon->GetName());
        return result;
    }

    // attach
    result = weapon->AttachToComponent(target, FAttachmentTransformRules::SnapToTargetIncludingScale, FName(*socket));

    return result;
}

bool UMeleeUtils::DetachWeapon(UDetectMelee *target, UPARAM(meta=(Bitmask, UseEnumValuesAsMaskValuesInEditor="true", BitmaskEnum=EAttackWeapon)) uint8 WeaponType, USceneComponent* weaponPtr)
{
    bool result = false;

    if (target == nullptr || target->GetOwner() == nullptr)
    {
        UE_LOG(LogTemp, Warning,TEXT("Target component or actor does not valid!"));
        return result;
    }

    TArray<TScriptInterface<IMeleeWeapon>> weapons;
    target->GetWeapons(weapons);
    for (auto w : weapons)
    {
        if (w && w->IsTargetWeapon(WeaponType))
        {
            auto targetWeapon = dynamic_cast<USceneComponent*>(w.GetObject());
            if (targetWeapon && (weaponPtr == nullptr || weaponPtr == targetWeapon))
            {
                targetWeapon->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
                // TODO Create a weapon containner to contains detached weapon
                targetWeapon->DestroyComponent();
                result = true;
                break;
            }
        }
    }

    return result;
}

bool UMeleeUtils::BriefPauseMontage(AActor* target, float duration)
{
    auto montageInst = GetMontageInstance(target);
    if (montageInst && montageInst->IsPlaying())
    {
        FTimerHandle handle;
        montageInst->Pause();
        target->GetWorld()->GetTimerManager().SetTimer(handle, [target]() {
            auto _montageIns = GetMontageInstance(target);
            if (_montageIns && !_montageIns->IsPlaying())
            {
                _montageIns->SetPlaying(true);
            }
        }, duration, false);
        return true;
    }

    return false;
}

FAnimMontageInstance* UMeleeUtils::GetMontageInstance(AActor* target)
{
    if (target)
    {
        USkeletalMeshComponent *mesh = target->FindComponentByClass<USkeletalMeshComponent>();
        if (mesh)
        {
            UAnimInstance *animInst = mesh->GetAnimInstance();
            if (animInst)
            {
                return animInst->GetActiveMontageInstance();
            }
        }
    }

    return nullptr;
}

bool UMeleeUtils::CheckCameraLookUpAngle(float inAxis, float maxAngle, float minAngle, const FRotator &springArmRotator) 
{
    bool result = false;

    if (inAxis > 0 && (springArmRotator.Pitch + inAxis) < maxAngle)
    {
        result = true;
    }
    else if (inAxis < 0 && (springArmRotator.Pitch + inAxis) > minAngle)
    {
        result = true;
    }

    return result;
}

// template <typename InterfaceType>
// void UMeleeUtils::CallInterfaceFromActor(AActor *actor, TScriptInterface<InterfaceType> interface, UMeleeUtils::FCallInterfaceDelegate function , bool value)
// {
//     TArray<UObject *> inputers;
//     // UMeleeUtils::GetImplementFromActor(actor, (*(InterfaceType *)interface.GetInterface())::UClassType::StaticClass(), inputers);
//     UMeleeUtils::GetImplementFromActor(actor, interface, inputers);
//     if (inputers.Num() > 0)
//     {
//         for (auto inputObj : inputers)
//         {
//             function(inputObj, params);
//         }
//     }
//     inputers.Empty();
// }