#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "DetectSolution.generated.h"

USTRUCT(BlueprintType)
struct FDetectInfo
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=DetectInfo)
    AActor* detectOwner;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=DetectInfo)
    AActor* target;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=DetectInfo)
    FVector position;
};

UINTERFACE()
class MELEECORE_API UDetectSolution : public UInterface
{
    GENERATED_BODY()
};

class MELEECORE_API IDetectSolution
{
    GENERATED_BODY()
public:
    virtual void Init(FDataTableRowHandle table) = 0;
    virtual bool Detect(AActor* const current, FVector curDetectPos, FVector preDetectPos, TArray<FDetectInfo>& outDetect, const bool IsCheckPerframe) = 0;
    virtual void SetDebug(bool debugMode) = 0;
    virtual void OnEndDetection() {}
};