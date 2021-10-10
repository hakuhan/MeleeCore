#pragma once

#include "DetectTypes.generated.h"

UENUM(BlueprintType)
enum class EMeleeHurt : uint8
{
	VERY_LIGHT_HURT,
	LIGHT_HURT,
	MIDEUM_HURT,
	STRONG_HURT,
	VERY_STRONG_HURT,
	SUPER_HURT
};

UENUM(BlueprintType)
enum class ECombatHitResult : uint8
{
	NO_HIT,
	NO_INJURY_HIT,
	HURT_HIT,
	DEAD_HIT,
	DEFEND_HIT,
};
