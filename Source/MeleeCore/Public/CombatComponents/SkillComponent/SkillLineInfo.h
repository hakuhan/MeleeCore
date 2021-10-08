#pragma once

#include "CoreMinimal.h"
#include "SkillComponent/SkillInfo.h"
#include "Engine/DataTable.h"
#include "SkillLineInfo.generated.h"

UENUM(BlueprintType)
enum class ELineSwitchType : uint8
{
    LINE_SEQUENCE = 0 UMETA(DisplayName = "顺序"),
    LINE_INDIVIDUAL UMETA(DisplayName = "相互独立"),
};

USTRUCT(BlueprintType)
struct FSkillLineInfo
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=SkillLine)
    FString Name;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=SkillLine)
    ELineSwitchType SwitchType;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=SkillLine)
    TArray<FSkillInfo> SkillLine;
};