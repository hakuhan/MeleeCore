// Fill out your copyright notice in the Description page of Project Settings.

#include "MeleeDetection/DetectMelee.h"
#include "MeleeDetection/CollisionDetect.h"

// Sets default values for this component's properties
UDetectMelee::UDetectMelee()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDetectMelee::BeginDestroy()
{
	Super::BeginDestroy();
	m_MeleeWeapons.Empty();
	m_HitActorTemps.Empty();
}

// Called when the game starts
void UDetectMelee::BeginPlay()
{
	Super::BeginPlay();

	UpdateWeapon();

	// Init Detect solution
	auto detectSolution = m_DetectSolutionTable.GetRow<FDetectSolutionTable>("Find combat solution");
	if (detectSolution)
	{
		m_DetectType = detectSolution->DetectType;
		auto detectObj = NewObject<UObject>(this, detectSolution->Class);
		if (detectObj)
		{
			m_DetectSolution.SetObject(detectObj);
			m_DetectSolution.SetInterface(dynamic_cast<IDetectSolution*>(detectObj));
			m_DetectSolution->Init(detectSolution->DetectInfo);
		}

	}

	// Init hurt solution data
	auto solutionInfo = m_HurtSolutionTable.GetRow<FHurtSolutionTable>("Find combat solution");
	if (solutionInfo != nullptr)
	{
		m_DefaultHurtSolutionType = solutionInfo->solutionType;
		m_HurtSolutionType = m_DefaultHurtSolutionType;

		auto hurtObj = NewObject<UObject>(this, solutionInfo->solutionClass);
		auto hurtInfo = solutionInfo->hurtTable.GetRow<FHurt>("Find combat hurt");
		if (hurtObj && hurtInfo)
		{
			m_HurtSolution.SetObject(hurtObj);
			m_HurtSolution.SetInterface(dynamic_cast<ICombatSolution*>(hurtObj));
			m_HurtSolution->Init(*hurtInfo);

			// set defaul data
			m_DefaultHurt = hurtInfo->hurtType;
			m_Hurt = m_DefaultHurt;
		}
	}

	m_EffectComponent = dynamic_cast<UMeleeEffect*>(GetOwner()->GetComponentByClass(UMeleeEffect::StaticClass()));
}

void UDetectMelee::UpdateHurts(EMeleeHurt newHurt, EHurtType newSolution)
{
	if (m_HurtSolution == nullptr)
	{
		return;
	}

	FHurtSolutionTable* solutionInfo = m_HurtSolutionTable.GetRow<FHurtSolutionTable>("Find combat solution");
	if (newSolution != m_HurtSolutionType)
	{
		for (auto solutionName : m_HurtSolutionTable.DataTable->GetRowNames())
		{
			auto sRow = m_HurtSolutionTable.DataTable->FindRow<FHurtSolutionTable>(solutionName, "Find combat solution", true);
			if (sRow->solutionType == newSolution)
			{
				solutionInfo = sRow;
				auto solutionObj = NewObject<UObject>(this, sRow->solutionClass);
				m_HurtSolution.SetObject(nullptr);
				m_HurtSolution.SetObject(solutionObj);
				m_HurtSolution.SetInterface(dynamic_cast<ICombatSolution*>(solutionObj));
				m_HurtSolutionType = newSolution;
				break;
			}
		}
	}

	if (newHurt != m_Hurt)
	{
		FHurt newHurtInfo = m_HurtSolution->m_HurtInfo;
		auto hurtTable = solutionInfo->hurtTable.DataTable;
		if (!hurtTable)
		{
			UE_LOG(LogTemp, Error, TEXT("Hurt tale empty!"));
		}
		for (auto name : hurtTable->GetRowNames())
		{
			auto row = hurtTable->FindRow<FHurt>(name, "Find combat hurts", true);
			if (row->hurtType == newHurt)
			{
				newHurtInfo = *row;
				m_Hurt = newHurt;
				break;
			}
		}

		m_HurtSolution->UpdateHurts(newHurtInfo);
	}
}

void UDetectMelee::UpdateDetect(EDetectType type)
{
	if (type != m_DetectType)
	{
		for (auto solutionName : m_DetectSolutionTable.DataTable->GetRowNames())
		{
			auto sRow = m_DetectSolutionTable.DataTable->FindRow<FDetectSolutionTable>(solutionName, "Find Detect Solution", true);
			if (sRow->DetectType == type)
			{
				auto solutionObj = NewObject<UObject>(this, sRow->Class);
				m_DetectSolution.SetObject(nullptr);
				m_DetectSolution.SetObject(solutionObj);
				m_DetectSolution.SetInterface(dynamic_cast<IDetectSolution*>(solutionObj));
				m_DetectSolution->Init(sRow->DetectInfo);
				m_DetectType = type;
				break;
			}
		}
	}
}

void UDetectMelee::UpdateCollisionData(const FCollisionDetectData& data)
{
	if (m_DetectType == EDetectType::COLLISION && m_DetectSolution.GetObject())
	{
		auto _collisionObj = dynamic_cast<UCollisionDetect*>(m_DetectSolution.GetObject());
		if (_collisionObj)
		{
			_collisionObj->UpdateData(data);
		}
	}
}



void UDetectMelee::UpdateHurtRate(float rate)
{
	if (m_HurtSolution != nullptr)
	{
		m_HurtSolution->UpdateHurtRate(rate);
	}
}

void UDetectMelee::UpdateWeaponMask(uint8 weaponMask)
{
	m_WeaponMask = weaponMask;
}

void UDetectMelee::ResetHurts()
{
	UpdateHurts(m_DefaultHurt, m_DefaultHurtSolutionType);
}

// Called every frame
void UDetectMelee::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (m_IsDetecting && m_DetectTiming > 0)
	{
		m_DetectTiming -= DeltaTime;
		Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
		m_HitActorTemps.Empty();

		if (!m_DetectSolution.GetObject())
		{
			UE_LOG(LogTemp, Warning, TEXT("Detect solution of: %s is invalid"), *(GetOwner()->GetName()));
			return;
		}

		m_DetectSolution->SetDebug(m_IsDebug);

		// attack check
		for (int i = 0; i < m_MeleeWeapons.Num(); ++i)
		{
			if (!m_MeleeWeapons[i] || !m_MeleeWeapons[i]->IsTargetWeapon(m_WeaponMask)
				|| !m_MeleeWeapons[i]->IsWeaponEnabled())
			{
				continue;
			}

			// Get weapon info and data
			if (!m_MeleeWeapons[i]->GetInfo(m_WeaponInfoTemp))
			{
				continue;
			}
			if (!m_MeleeWeapons[i]->GetData(m_WeaponDataTemp))
			{
				continue;
			}

			for (int j = 0; j < m_WeaponInfoTemp.SocketNames.Num(); ++j)
			{
				// 1. Check every single socket
				// check slot location cache
				FVector crtLocation = m_MeleeWeapons[i]->GetDetectLocation(m_WeaponInfoTemp.SocketNames[j]);
				if (m_WeaponDataTemp.TempSocketLocation.Num() <= j)
				{
					m_WeaponDataTemp.TempSocketLocation.Add(crtLocation);
				}
				FVector preLocation = m_WeaponDataTemp.TempSocketLocation[j];

				if (m_DetectSolution->Detect(GetOwner(), crtLocation, preLocation, m_DetectTemp, true))
				{
					for (auto _detect : m_DetectTemp)
					{
						ExecuteHit(_detect);
					}
				}

				// 2. Check trace between sokets
				int k = j + 1;
				if (k < m_WeaponInfoTemp.SocketNames.Num())
				{
					auto nextSocketLocation = m_MeleeWeapons[i]->GetDetectLocation(m_WeaponInfoTemp.SocketNames[k]);
					if (m_DetectSolution->Detect(GetOwner(), crtLocation, nextSocketLocation, m_DetectTemp, false))
					{
						for (auto _detect : m_DetectTemp)
						{
							ExecuteHit(_detect);
						}
					}
				}

				m_WeaponDataTemp.TempSocketLocation[j] = crtLocation;
			}

			// Update data
			m_MeleeWeapons[i]->SetData(m_WeaponDataTemp);
		}
	}
}

void UDetectMelee::StartDetection(float duration)
{
	if (m_HurtSolution != nullptr)
	{
		ICombatSolution::Execute_OnStartDetection(m_HurtSolution.GetObject());
	}
	m_IsDetecting = true;
	m_DetectTiming = duration;

	ResetData();
}

void UDetectMelee::ResetData()
{
	for (int i = 0; i < m_MeleeWeapons.Num(); ++i)
	{
		if (m_MeleeWeapons[i] == nullptr || !m_MeleeWeapons[i]->GetData(m_WeaponDataTemp))
		{
			continue;
		}
		m_WeaponDataTemp.TempSocketLocation.Empty();
		m_MeleeWeapons[i]->SetData(m_WeaponDataTemp);
	}
	if (m_EffectComponent)
	{
		m_EffectComponent->ResetData();
	}
}

void UDetectMelee::EndDetection()
{
	if (m_HurtSolution != nullptr)
	{
		ICombatSolution::Execute_OnEndDetection(m_HurtSolution.GetObject());
	}
	if (m_DetectSolution != nullptr)
	{
		m_DetectSolution->OnEndDetection();
	}

	m_IsDetecting = false;
}

void UDetectMelee::UpdateWeapon()
{
	m_MeleeWeapons.Empty();

	TArray<UObject*> weapons;
	UMeleeUtils::GetImplementFromActor(GetOwner(), UMeleeWeapon::StaticClass(), weapons, false);
	for (auto _obj : weapons)
	{
		TScriptInterface<IMeleeWeapon> _weapon;
		_weapon.SetObject(_obj);
		_weapon.SetInterface(dynamic_cast<IMeleeWeapon*>(_obj));;
		m_MeleeWeapons.Add(_weapon);
	}
}

void UDetectMelee::ExecuteHit(FDetectInfo& hit)
{
	AActor* actor = hit.target;
	if (m_HitActorTemps.Contains(actor))
	{
		return;
	}
	m_HitActorTemps.Add(actor);

	ECombatHitResult _hitResult = ECombatHitResult::NO_HIT;
	if (m_HurtSolution != nullptr)
	{
		ICombatSolution::Execute_OnHit(m_HurtSolution.GetObject(), GetOwner(), actor, _hitResult);
	}

	if (m_EffectComponent != nullptr)
	{
		m_EffectComponent->OnCombatHitEffect(hit, _hitResult);
	}
}
