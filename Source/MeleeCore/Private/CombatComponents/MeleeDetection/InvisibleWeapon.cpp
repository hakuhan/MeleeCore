#include "MeleeDetection/InvisibleWeapon.h"

void UInvisibleWeapon::BeginPlay()
{
	Super::BeginPlay();

	m_Info.SocketNames.Empty();
	m_Info.SocketNames.Add(m_SocketName);
}