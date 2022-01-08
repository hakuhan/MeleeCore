// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Actor_Countdown.generated.h"

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_DELEGATE(FCountdownSignature);
// typedef void (*CountdownCallback)()

USTRUCT()
struct FCountdownInfo
{
	GENERATED_BODY()

public:
	UObject *caller;
	float time;
	float timeBuffer = 0;
	FCountdownSignature callback;

	FCountdownInfo()
	{
	}

	FCountdownInfo(UObject *caller, FCountdownSignature &callback, float time)
		: caller(caller), time(time), callback(callback)
	{
		timeBuffer = 0;
	}

	bool operator==(const FCountdownInfo &c1)
	{
		return this->caller == c1.caller && this->callback == c1.callback;
	}

	bool IsTimeOver()
	{
		return time <= timeBuffer;
	}

	bool Equals(const UObject *objCaller, const FCountdownSignature &callbackFunc)
	{
		return this->caller == objCaller && this->callback == callbackFunc;
	}
};

UCLASS()
class MELEECORE_API AActor_Countdown : public AActor
{
	GENERATED_BODY()

public:
	TArray<FCountdownInfo *> m_Countdowns;

public:
	// Sets default values for this actor's properties
	AActor_Countdown();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void StartCountdown(UObject *caller, FCountdownSignature &callback, float time);
	void RemoveCountdown(UObject *caller, const FCountdownSignature &callback);
	bool IsCounting(UObject *caller, FCountdownSignature &callback);
};