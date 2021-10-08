// Fill out your copyright notice in the Description page of Project Settings.

#include "RemoteDetection/DetectRemote.h"
#include "MeleeDetection/DetectSolution.h"

// Sets default values for this component's properties
UDetectRemote::UDetectRemote()
{
	PrimaryComponentTick.bCanEverTick = true;
}

UDetectRemote::~UDetectRemote()
{
	m_Detectors.Empty();
}

UObject *UDetectRemote::Launch(const FTransform& remoteTransform, const FVector& launchDirection, const FString &remoteName, AActor *attachActor)
{
	UObject *outDetector = nullptr;
	// Find info
	if (m_Infos.DataTable)
	{
		FDetectRemoteInfo *info = nullptr;
		for (auto rowName : m_Infos.DataTable->GetRowNames())
		{
			if (remoteName == rowName.ToString())
			{
				info = m_Infos.DataTable->FindRow<FDetectRemoteInfo>(rowName, "Find remote", true);
				break;
			}
		}

		// Create detector
		if (info && info->DetectorClass)
		{
			UObject *detectorObj = nullptr;
			switch (info->Type)
			{
			case ERemoteDetector::ATTACH_DETECTOR:
				if (attachActor)
				{
					detectorObj = NewObject<UObject>(attachActor, info->DetectorClass);
					USceneComponent *detectorComp = dynamic_cast<USceneComponent *>(detectorObj);
					if (detectorComp)
					{
						detectorComp->RegisterComponent();
						detectorComp->AttachToComponent(attachActor->GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
						UE_LOG(LogTemp, Warning, TEXT("Attach component name:%s"), *info->DetectorClass->GetName())
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("Attack actor is not valid!"))
					}
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Attack actor is not valid!"))
				}

				break;

			case ERemoteDetector::MOVABLE_DETECTOR:
				detectorObj = GetOwner()->GetWorld()->SpawnActor<AActor>(info->DetectorClass, remoteTransform);
				break;

			default:
				break;
			}

			if (detectorObj)
			{
				outDetector = detectorObj;
				if (detectorObj)
				{
					m_Detectors.Add(outDetector);
					IRemoteDetector *detectInterface = dynamic_cast<IRemoteDetector *>(detectorObj);

					// Bind detector callback
					if (detectInterface)
					{
						FOnRemoteHit hitEvent;
						hitEvent.BindDynamic(this, &UDetectRemote::OnHit);
						IRemoteDetector::Execute_InitData(detectorObj, info->DetectInfoTable, launchDirection, hitEvent);
						IRemoteDetector::Execute_StartDetection(detectorObj);
					}
				}
			}

		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Remote detect info invalid!"))
		}
	}

	return outDetector;
}

// Called every frame
void UDetectRemote::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Check remote validations
	for (int i = m_Detectors.Num() - 1; i >= 0; --i)
	{
		if (m_Detectors[i] == nullptr)
		{
			m_Detectors.RemoveAt(i);
		}
		else if (IRemoteDetector::Execute_IsFinished(m_Detectors[i]))
		{
			AActor* _actor = dynamic_cast<AActor*>(m_Detectors[i]);
			if (_actor)
			{
				_actor->Destroy();
			}
			else
			{
				m_Detectors[i]->MarkPendingKill();
			}
			m_Detectors.RemoveAt(i);
		}
	}
}

void UDetectRemote::OnHit(const FDetectInfo &info, const FHurt &hurt)
{
	// Compute hit
	if (info.target)
	{
		TArray<UObject *> reacters;
		UMeleeUtils::GetImplementFromActor(info.target, UCombatReaction::StaticClass(), reacters);
		if (reacters.Num() > 0)
		{
			for (auto reacter : reacters)
			{
				ICombatReaction::Execute_OnMeleeHitted(reacter, hurt.hurts);
				ICombatReaction::Execute_GetHitResult(reacter);
			}
		}
	}
}