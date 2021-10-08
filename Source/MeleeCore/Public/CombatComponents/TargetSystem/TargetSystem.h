#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Components/TimelineComponent.h"
#include "Engine/DataTable.h"
#include "Curves/CurveFloat.h"

#include "TargetSystem.generated.h"

class USphereComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSelectTargetDelegate, AActor*, owner, AActor*, target);

UENUM(BlueprintType)
enum class ESwitchToRule : uint8
{
    TARGET_Lock UMETA(DisplayName="锁定"),
    TARGET_LookAt UMETA(DisplayName="变换视角"),
    TARGET_FaceTo UMETA(DisplayName="角色面向"),
    TARGET_LockAndFaceTo UMETA(DisplayName="锁定并面向"),
    TARGET_LookAtAndFaceTo UMETA(DisplayName="变换视角并面向")
};

UENUM(BlueprintType)
enum class EBestFindingTagetRule : uint8
{
    FIND_DistanceFirst UMETA(DisplayName="距离优先"),
    FIND_AngleFirst UMETA(DisplayName="角度优先"),
    FIND_Banace UMETA(DisplayName="两者平衡"),
    FIND_DistanceGold UMETA(DisplayName="距离优先黄金比例"),
    FIND_AngleGold UMETA(DisplayName="角度优先黄金比例"),
};


USTRUCT(BlueprintType)
struct MELEECORE_API FTargetInfo : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere ,BlueprintReadWrite, Category=TargetSystem)
    FName CollisionPresetName = TEXT("PlayerCombat");

    UPROPERTY(EditAnywhere ,BlueprintReadWrite, Category=TargetSystem)
    ESwitchToRule SwitchingRule;

    UPROPERTY(EditAnywhere ,BlueprintReadWrite, Category=TargetSystem)
    EBestFindingTagetRule BestTargetRule;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=TargetSystem)
    // 检查耗时 (秒)
    float m_CheckingDuration = 0.2;

    UPROPERTY(EditAnywhere ,BlueprintReadWrite, Category=TargetSystem)
    // 距离权重
    float DistanceWeight = 10;

    UPROPERTY(EditAnywhere ,BlueprintReadWrite, Category=TargetSystem)
    // 角度权重
    float AngleWeight = 5;

    UPROPERTY(EditAnywhere ,BlueprintReadWrite, Category=TargetSystem)
    TArray<FName> TargetTags;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=TargetSystem)
    // 查找半径
    float DetectRadius = 10;    

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=TargetSystem)
    // 查找高度范围
    float DetectHightRange = 2;

    UPROPERTY(EditAnywhere ,BlueprintReadWrite, Category=TargetSystem)
    // 危险点(优先考虑危险点, 单位：角度，以玩家正前方为标准， 左为负，右为正，范围是-180到180)
    TArray<float> DangerousPoints;

    UPROPERTY(EditAnywhere ,BlueprintReadWrite, Category=TargetSystem)
    // 危险区域范围(角度)
    float DangerousAngleRange = 10;
    
    UPROPERTY(EditAnywhere ,BlueprintReadWrite, Category=TargetSystem)
    // 危险距离（米）
    float DangerousDistance = 2;

    UPROPERTY(EditAnywhere ,BlueprintReadWrite, Category=TargetSystem)
    // 危险高度范围（米）
    float DangerousHightRange = 1;

    UPROPERTY(EditAnywhere ,BlueprintReadWrite, Category=TargetSystem)
    // 如果之前目标在专注范围内，并且相当近，则优于危险区域外的其他目标
    bool bPreTargetFirst = true;

    UPROPERTY(EditAnywhere ,BlueprintReadWrite, Category=TargetSystem)
    // 专注区域范围(正前方)
    float FocusRange = 90;

    UPROPERTY(EditAnywhere ,BlueprintReadWrite, Category=TargetSystem)
    // 专注区域距离（单位米）
    float FocusDistance = 5;

    UPROPERTY(EditAnywhere ,BlueprintReadWrite, Category=TargetSystem)
    // 专注区域高度范围（单位米）
    float FocusHightRange = 1;

    UPROPERTY(EditAnywhere ,BlueprintReadWrite, Category=TargetSystem)
    // 相机旋转时间
    float SwitchCameraDuration = 1;

    UPROPERTY(EditAnywhere ,BlueprintReadWrite, Category=TargetSystem)
    // 相机移动速度曲线
    UCurveFloat* CameraSpeedCurve;

    UPROPERTY(EditAnywhere ,BlueprintReadWrite, Category=TargetSystem)
    // 曲线X范围
    float CameraCurveXDuration = 1;

    UPROPERTY(EditAnywhere ,BlueprintReadWrite, Category=TargetSystem)
    // 曲线Y范围
    float CameraCurveYDuration = 1;
};

USTRUCT(BlueprintType)
struct MELEECORE_API FTargetData
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=TargetSystem)
    AActor* CurrentTarget;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=TargetSystem)
    TArray<AActor*> AvailableTargets;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=TargetSystem)
    float DetectRadius;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=TargetSystem)
    ESwitchToRule SwitchingRule;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=TargetSystem)
    bool bLocking = false;

    UPROPERTY(BlueprintReadWrite)
    bool bAnimating = false;

    UPROPERTY()
    float AnimTimeBuffer = 0;

    UPROPERTY(BlueprintReadWrite)
    FRotator CameraStartRotator;

    UPROPERTY(BlueprintReadWrite)
    FRotator CameraTargetRotator;

    void Reset()
    {
        CurrentTarget = nullptr;
        AvailableTargets.Empty();
        bAnimating = false;
        bLocking = false;
        AnimTimeBuffer = 0;
    }
};

UCLASS(Blueprintable, ClassGroup = (Mind), meta = (BlueprintSpawnableComponent))
class MELEECORE_API UTargetSystem : public USceneComponent
{
    GENERATED_BODY()
public:
    UTargetSystem();
	virtual void BeginPlay();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UFUNCTION(BlueprintCallable)
    void UpdateInfo(const FTargetInfo& info);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    AActor* FindBestTarget(const TArray<AActor*>& avaliables);
    virtual AActor* FindBestTarget_Implementation(const TArray<AActor*>& avaliables);

    UFUNCTION(BlueprintCallable)
    void SwitchTarget(bool useDefaultSetting = true, ESwitchToRule newRule = ESwitchToRule::TARGET_FaceTo, float newDetectRadius = 5.0f);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    bool LockOnTarget();
    virtual bool LockOnTarget_Implementation();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    bool LookAtTarget();
    virtual bool LookAtTarget_Implementation();

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    bool FaceToTarget();
    virtual bool FaceToTarget_Implementation();

    UFUNCTION()
    void OnOverlabBegin(class UPrimitiveComponent* Comp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    void UnLockTarget();
    void UnLockTarget_Implementation();

    UFUNCTION(BlueprintCallable)
    bool IsLocking();
    
public:
    UPROPERTY(BlueprintReadWrite)
    FOnSelectTargetDelegate m_OnSeleteTarget;

    UPROPERTY(EditAnywhere)
    FDataTableRowHandle m_InfoTable;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    FTargetInfo m_Info;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    FTargetData m_Data;

    UPROPERTY()
    USphereComponent* m_CheckingComponent;

    UPROPERTY()
    // to prevent double set info from beginplay
    bool m_IsUpdatedInfo = false;

    UPROPERTY(EditAnywhere)
    bool m_bDebug = false;
  
    UPROPERTY()
    UTimelineComponent* m_Timeline;
};

