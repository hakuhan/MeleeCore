#pragma once


#include "CoreMinimal.h"
#include "SkillInfo.generated.h"

UENUM(BlueprintType)
enum class ESkillType : uint8
{
    SKILL_NORMAL UMETA(DisplayName="普通技能"),
    SKILL_SUPER UMETA(DisplayName="超级技能"),
};

// skill data
USTRUCT(BlueprintType)
struct FSkillInfo
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString Name;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    ESkillType SkillType;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    bool Loop;

    UPROPERTY(BlueprintReadWrite, EditAnywhere)
    UAnimMontage* Montage;

    FSkillInfo()
    {}

    FSkillInfo(const FSkillInfo& other)
    {
        Name = other.Name;
        SkillType = other.SkillType;
        Loop = other.Loop;
        Montage = other.Montage;
    }

    FSkillInfo(FString _name, ESkillType _type, bool _loop, UAnimMontage* _montage)
    {
        Name = _name;
        SkillType = _type;
        Loop = _loop;
        Montage = _montage;
    }

    inline bool operator==(const FSkillInfo& other)
    {
        return Name == other.Name
                && SkillType == other.SkillType
                && Montage == other.Montage;
    }

    inline bool operator!=(const FSkillInfo& other)
    { 
        return !(*this == other); 
    }
};
