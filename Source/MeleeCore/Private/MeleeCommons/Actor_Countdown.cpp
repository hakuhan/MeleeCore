// Fill out your copyright notice in the Description page of Project Settings.

#include "Actor_Countdown.h"

// Sets default values
AActor_Countdown::AActor_Countdown()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AActor_Countdown::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AActor_Countdown::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	for (int i = m_Countdowns.Num() - 1; i >= 0; --i)
	{
		auto info = m_Countdowns[i];
		if (!info->caller)
		{
			m_Countdowns.RemoveAt(i);
			continue;
		}

		info->timeBuffer += DeltaTime;
		if (info->IsTimeOver())
		{
			if (info->callback.IsBound())
			{
				info->callback.Execute();
			}
			m_Countdowns.RemoveAt(i);
		}
	}
}

void AActor_Countdown::StartCountdown(UObject *caller, FCountdownSignature &callback, float time)
{
	auto info = new FCountdownInfo(caller, callback, time);
	if (!m_Countdowns.Contains(info))
	{
		m_Countdowns.Add(info);
	}
}

void AActor_Countdown::RemoveCountdown(UObject *caller, const FCountdownSignature &callback)
{
	for (int i = m_Countdowns.Num() - 1; i >= 0; --i)
	{
		if (m_Countdowns[i]->Equals(caller, callback))
		{
			m_Countdowns.RemoveAt(i);
			break;
		}
	}
}

bool AActor_Countdown::IsCounting(UObject *caller, FCountdownSignature &callback)
{
	auto info = new FCountdownInfo(caller, callback, 0);
	return m_Countdowns.Contains(info);
}
