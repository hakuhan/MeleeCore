#pragma once

#include "MeleeDetection/DetectSolution.h"
#include "MeleeDetection/CollisionDetectInfo.h"
#include "CombatSolution.h"
#include "RemoteDetectorInterface.generated.h"

DECLARE_DYNAMIC_DELEGATE_TwoParams(FOnRemoteHit, const FDetectInfo&, detectInfo, const FHurt&, hurt);

USTRUCT(BlueprintType)
struct FBlueprintRemoteEvent
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=RemoteDetection)
    FOnRemoteHit HitEvent;
};

UINTERFACE(Blueprintable)
class MELEECORE_API URemoteDetector : public UInterface
{
    GENERATED_BODY()
};

class MELEECORE_API IRemoteDetector
{
    GENERATED_BODY()
public:

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "RemoteDetection")
    void InitData(const FDataTableRowHandle& infoTable, const FVector& direction, const FOnRemoteHit& callback);
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "RemoteDetection")
    void StartDetection();
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "RemoteDetection")
    void StopDetection();
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "RemoteDetection")
    void UpdateShape(ECollisionDetectType shape);
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "RemoteDetection")
    void UpdateSize(FVector size);
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "RemoteDetection")
    void PlayHitEffect();
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "RemoteDetection")
    bool IsFinished();
};