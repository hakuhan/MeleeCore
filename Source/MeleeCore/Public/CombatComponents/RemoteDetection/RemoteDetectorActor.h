#pragma once

#include "CoreMinimal.h"
#include "RemoteDetectorInterface.h"

#include "RemoteDetectorActor.generated.h"

class UCollisionDetect;
class URayDetect;

UCLASS()
class MELEECORE_API ARemoteDetectorActor : public AActor, public IRemoteDetector
{
    GENERATED_BODY()
public:
    ARemoteDetectorActor()
        : m_bFinish(true)
    {}

    virtual void InitData_Implementation(const FDataTableRowHandle& infoTable, const FVector& direction, const FOnRemoteHit& callback);
    virtual bool IsFinished_Implementation();
    virtual void StartDetection_Implementation();
    virtual void StopDetection_Implementation();
    virtual void UpdateSize_Implementation(FVector size);
    virtual void UpdateShape_Implementation(ECollisionDetectType shape);


	virtual void Tick(float DeltaTime) override;
	
    UFUNCTION(BlueprintCallable)
    void OnHit(const FDetectInfo& detectInfo);

public:
    FOnRemoteHit m_HitDelegate;
    UPROPERTY(BlueprintReadWrite)
    FHurt m_Hurt;
    UPROPERTY(BlueprintReadWrite)
    FVector m_Direction;
    UPROPERTY(EditAnywhere)
    FVector m_Size = FVector(10, 10, 10);
    UPROPERTY(BlueprintReadWrite)
    bool m_bFinish = true;
    UPROPERTY(BlueprintReadWrite)
    UCollisionDetect* m_collisionDetector;
    UPROPERTY(BlueprintReadWrite)
    URayDetect* m_rayDetector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    // -1为没有限制，单位秒
    float m_LifeDuration = -1;
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    // 当前存活时间
    float m_LifeBuffer = 0;

    UPROPERTY(EditAnywhere)
    bool m_DestroyWhenHurt = true;
    UPROPERTY(EditAnywhere)
    bool m_bDebug = false;
};