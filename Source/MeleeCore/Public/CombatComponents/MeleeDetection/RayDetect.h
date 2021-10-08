#pragma once

#include "CoreMinimal.h"
#include "DetectSolution.h"
#include "Kismet/KismetSystemLibrary.h"

#include "RayDetect.generated.h"

USTRUCT(BlueprintType)
struct FRayDetectInfo : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=RayDetect)
    bool IsShowTrace;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=RayDetect)
    FLinearColor RayColor = FLinearColor::Green;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=RayDetect)
    FLinearColor HittedColor = FLinearColor::Red;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RayDetect, meta = (Bitmask, BitmaskEnum = "ECollisionChannel"))
	TEnumAsByte<ECollisionChannel> Channel;
};


UCLASS()
class MELEECORE_API URayDetect : public UObject, public IDetectSolution
{
    GENERATED_BODY()

    UPROPERTY()
    FRayDetectInfo m_Info;

    UPROPERTY()
    TArray<FHitResult> m_Hits;
    UPROPERTY()
	TArray<AActor *> m_IgnoreActors;

public:
    void Init(FDataTableRowHandle table) override
    {
        auto infoPtr = table.GetRow<FRayDetectInfo>("Find Ray Detect Solution");
        if (infoPtr)
        {
            m_Info = *infoPtr;
        }
    };

    bool Detect(AActor* const current, FVector curDetectPos, FVector preDetectPos, TArray<FDetectInfo>& outDetect, const bool IsCheckPerframe) override
    {
        bool result = false;
        m_Hits.Empty();
        outDetect.Empty();
        auto debugTraceType = m_Info.IsShowTrace ? EDrawDebugTrace::Type::ForDuration : EDrawDebugTrace::Type::None;
        auto channel = UEngineTypes::ConvertToTraceType(m_Info.Channel);

        UKismetSystemLibrary::LineTraceMulti(current->GetWorld(), preDetectPos, curDetectPos, channel, true, m_IgnoreActors, debugTraceType, m_Hits, true, m_Info.RayColor, m_Info.HittedColor);
        for (auto hit : m_Hits)
        {
            FDetectInfo _detectInfo;
            _detectInfo.detectOwner = current;
            _detectInfo.target = hit.GetActor();
            _detectInfo.position = hit.Location;
            outDetect.Add(_detectInfo);
            result = true;
        }

        return result;
    }

    void SetDebug(bool debugMode) override
    {
        m_Info.IsShowTrace = debugMode;
    }
};
