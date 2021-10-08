#include "MeleeDetection/SkeletalWeapon.h"

void USkeletalWeapon::BeginPlay()
{
	Super::BeginPlay();

	m_Info.SocketNames.Empty();
	m_Info.SocketNames.Append(GetAllSocketNames());
}