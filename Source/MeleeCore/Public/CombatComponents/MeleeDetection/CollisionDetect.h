#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Components/ShapeComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "DetectSolution.h"
#include "BodySetupEnums.h"
#include "CollisionDetectInfo.h"

#include "CollisionDetect.generated.h"

USTRUCT(BlueprintType)
struct MELEECORE_API FCollisionDetectData
{
    GENERATED_USTRUCT_BODY()
public:
    FCollisionDetectData()
    {
        Type = ECollisionDetectType::CAPSULE_DETECT;
        DurationFrame = 6;
        Color = FColor(100, 255, 100, 255);
        Size = FVector(10, 10, 10);
        Rotation = FRotator(0);
        Scale = 1;
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ECollisionDetectType Type;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int DurationFrame;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FColor Color;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector Size;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FRotator Rotation;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Scale;
};


UCLASS()
class MELEECORE_API UCollisionDetect : public UObject, public IDetectSolution
{
    GENERATED_BODY()
public:
    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    FCollisionDetectInfo m_Info;
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    FCollisionDetectData m_Data;

    TArray<UShapeComponent*> m_CheckingCollisions;

    // Check first spawn
    TArray<UShapeComponent*> m_LastFrameSpawnedCollisions;
    TArray<int> m_DelayDestroyFrames;

    TArray<FDetectInfo> m_HittedInfos;


    void Init(FDataTableRowHandle table) override;

    virtual bool Detect(AActor* const current, FVector curDetectPos, FVector preDetectPos, TArray<FDetectInfo>& outDetect, const bool IsCheckPerframe) override;

    void SetDebug(bool debugMode) override
    {
        m_Info.IsShowCollision = debugMode;
    }

    void UpdateData(const FCollisionDetectData& data)
    {
        m_Data = data;
    }

	void CheckCollisionWhenSpawing();
    
    UFUNCTION(BlueprintCallable)
    void CleanCollision();

    void OnEndDetection() override;
    void DestroyCheck();
    void DestroyCollision(UShapeComponent* target);

    UFUNCTION()
    void OnOverlapBegin(class UPrimitiveComponent* Comp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};